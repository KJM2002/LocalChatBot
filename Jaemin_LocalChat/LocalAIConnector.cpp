#include "LocalAIConnector.h"

#include <array>
#include <cstdio>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

bool startsWithText(const std::string& text, const std::string& prefix)
{
    if (prefix.length() > text.length())
    {
        return false;
    }

    return text.substr(0, prefix.length()) == prefix;
}

std::string trimCommandText(const std::string& text)
{
    const std::string spaces = " \t";
    const std::string::size_type first = text.find_first_not_of(spaces);

    if (first == std::string::npos)
    {
        return "";
    }

    const std::string::size_type last = text.find_last_not_of(spaces);
    return text.substr(first, last - first + 1);
}

bool isAICommand(const std::string& input)
{
    return input == "ai" || startsWithText(input, "ai ");
}

bool isAIStatusCommand(const std::string& input)
{
    return input == "aistatus";
}

bool isAIModelCommand(const std::string& input)
{
    return input == "aimodel";
}

bool isSetAIModelCommand(const std::string& input)
{
    return input == "setaimodel" || startsWithText(input, "setaimodel ");
}

std::string extractAIPrompt(const std::string& input)
{
    const std::string aiPrefix = "ai ";

    if (!startsWithText(input, aiPrefix))
    {
        return "";
    }

    return trimCommandText(input.substr(aiPrefix.length()));
}

std::string extractAIModelName(const std::string& input)
{
    const std::string modelPrefix = "setaimodel ";

    if (!startsWithText(input, modelPrefix))
    {
        return "";
    }

    return trimCommandText(input.substr(modelPrefix.length()));
}

std::string executeCommandAndCaptureOutput(const std::string& command)
{
#ifdef _WIN32
    SECURITY_ATTRIBUTES securityAttributes;
    securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
    securityAttributes.bInheritHandle = TRUE;
    securityAttributes.lpSecurityDescriptor = nullptr;

    HANDLE readPipe = nullptr;
    HANDLE writePipe = nullptr;

    if (!CreatePipe(&readPipe, &writePipe, &securityAttributes, 0))
    {
        return "";
    }

    if (!SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0))
    {
        CloseHandle(readPipe);
        CloseHandle(writePipe);
        return "";
    }

    STARTUPINFOW startupInfo;
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_USESTDHANDLES;
    startupInfo.hStdOutput = writePipe;
    startupInfo.hStdError = writePipe;
    startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

    PROCESS_INFORMATION processInformation;
    ZeroMemory(&processInformation, sizeof(processInformation));

    const std::string commandLineText = "cmd.exe /C " + command;
    const int wideLength = MultiByteToWideChar(CP_UTF8, 0, commandLineText.c_str(), -1, nullptr, 0);

    if (wideLength <= 0)
    {
        CloseHandle(readPipe);
        CloseHandle(writePipe);
        return "";
    }

    std::vector<wchar_t> commandLine(static_cast<std::size_t>(wideLength));
    MultiByteToWideChar(CP_UTF8, 0, commandLineText.c_str(), -1, commandLine.data(), wideLength);

    const BOOL processCreated = CreateProcessW(
        nullptr,
        commandLine.data(),
        nullptr,
        nullptr,
        TRUE,
        CREATE_NO_WINDOW,
        nullptr,
        nullptr,
        &startupInfo,
        &processInformation);

    CloseHandle(writePipe);

    if (!processCreated)
    {
        CloseHandle(readPipe);
        return "";
    }

    std::array<char, 256> buffer;
    std::string output;
    DWORD bytesRead = 0;

    while (ReadFile(readPipe, buffer.data(), static_cast<DWORD>(buffer.size()), &bytesRead, nullptr) && bytesRead > 0)
    {
        output.append(buffer.data(), bytesRead);
    }

    WaitForSingleObject(processInformation.hProcess, INFINITE);

    CloseHandle(processInformation.hThread);
    CloseHandle(processInformation.hProcess);
    CloseHandle(readPipe);

    return output;
#else
    std::array<char, 256> buffer;
    std::string output;

    FILE* pipe = popen(command.c_str(), "r");

    if (pipe == nullptr)
    {
        return "";
    }

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr)
    {
        output += buffer.data();
    }

    pclose(pipe);

    return output;
#endif
}

std::string escapeJsonString(const std::string& text)
{
    std::string escapedText;

    for (char character : text)
    {
        if (character == '"')
        {
            escapedText += "\\\"";
        }
        else if (character == '\\')
        {
            escapedText += "\\\\";
        }
        else if (character == '\n')
        {
            escapedText += "\\n";
        }
        else if (character == '\r')
        {
            escapedText += "\\r";
        }
        else if (character == '\t')
        {
            escapedText += "\\t";
        }
        else
        {
            escapedText += character;
        }
    }

    return escapedText;
}

std::string buildOllamaGenerateCommand(const std::string& prompt, const std::string& modelName)
{
    const std::string escapedPrompt = escapeJsonString(prompt);
    const std::string escapedModelName = escapeJsonString(modelName);

    std::string jsonBody = "{\\\"model\\\":\\\"" + escapedModelName;
    jsonBody += "\\\",\\\"prompt\\\":\\\"" + escapedPrompt;
    jsonBody += "\\\",\\\"stream\\\":false}";

    std::string command = "curl -s -X POST \"http://localhost:11434/api/generate\" ";
    command += "-H \"Content-Type: application/json\" ";
    command += "-d \"" + jsonBody + "\" 2>&1";

    return command;
}

int getHexDigitValue(char character)
{
    if (character >= '0' && character <= '9')
    {
        return character - '0';
    }

    if (character >= 'a' && character <= 'f')
    {
        return character - 'a' + 10;
    }

    if (character >= 'A' && character <= 'F')
    {
        return character - 'A' + 10;
    }

    return -1;
}

bool readUnicodeEscapeCodeUnit(const std::string& text, std::string::size_type hexStart, unsigned int& codeUnit)
{
    if (hexStart + 4 > text.length())
    {
        return false;
    }

    codeUnit = 0;

    for (std::string::size_type index = hexStart; index < hexStart + 4; index++)
    {
        const int digitValue = getHexDigitValue(text[index]);

        if (digitValue < 0)
        {
            return false;
        }

        codeUnit = codeUnit * 16 + static_cast<unsigned int>(digitValue);
    }

    return true;
}

bool isHighSurrogate(unsigned int codeUnit)
{
    return codeUnit >= 0xD800 && codeUnit <= 0xDBFF;
}

bool isLowSurrogate(unsigned int codeUnit)
{
    return codeUnit >= 0xDC00 && codeUnit <= 0xDFFF;
}

unsigned int combineSurrogatePair(unsigned int highSurrogate, unsigned int lowSurrogate)
{
    return 0x10000 + ((highSurrogate - 0xD800) * 0x400) + (lowSurrogate - 0xDC00);
}

void appendUtf8CodePoint(std::string& text, unsigned int codePoint)
{
    if (codePoint <= 0x7F)
    {
        text += static_cast<char>(codePoint);
    }
    else if (codePoint <= 0x7FF)
    {
        text += static_cast<char>(0xC0 | ((codePoint >> 6) & 0x1F));
        text += static_cast<char>(0x80 | (codePoint & 0x3F));
    }
    else if (codePoint <= 0xFFFF)
    {
        text += static_cast<char>(0xE0 | ((codePoint >> 12) & 0x0F));
        text += static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F));
        text += static_cast<char>(0x80 | (codePoint & 0x3F));
    }
    else if (codePoint <= 0x10FFFF)
    {
        text += static_cast<char>(0xF0 | ((codePoint >> 18) & 0x07));
        text += static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F));
        text += static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F));
        text += static_cast<char>(0x80 | (codePoint & 0x3F));
    }
}

std::string extractResponseField(const std::string& jsonText)
{
    const std::string responseKey = "\"response\":\"";
    const std::string::size_type responseStart = jsonText.find(responseKey);

    if (responseStart == std::string::npos)
    {
        return "";
    }

    std::string response;
    bool isEscaped = false;

    for (std::string::size_type index = responseStart + responseKey.length(); index < jsonText.length(); index++)
    {
        const char character = jsonText[index];

        if (isEscaped)
        {
            if (character == 'n')
            {
                response += '\n';
            }
            else if (character == 'r')
            {
                response += '\r';
            }
            else if (character == 't')
            {
                response += '\t';
            }
            else if (character == '"')
            {
                response += '"';
            }
            else if (character == '\\')
            {
                response += '\\';
            }
            else if (character == 'u')
            {
                unsigned int codeUnit = 0;

                if (readUnicodeEscapeCodeUnit(jsonText, index + 1, codeUnit))
                {
                    index += 4;

                    if (isHighSurrogate(codeUnit)
                        && index + 6 < jsonText.length()
                        && jsonText[index + 1] == '\\'
                        && jsonText[index + 2] == 'u')
                    {
                        unsigned int lowSurrogate = 0;

                        if (readUnicodeEscapeCodeUnit(jsonText, index + 3, lowSurrogate) && isLowSurrogate(lowSurrogate))
                        {
                            appendUtf8CodePoint(response, combineSurrogatePair(codeUnit, lowSurrogate));
                            index += 6;
                        }
                        else
                        {
                            appendUtf8CodePoint(response, codeUnit);
                        }
                    }
                    else
                    {
                        appendUtf8CodePoint(response, codeUnit);
                    }
                }
                else
                {
                    response += "\\u";
                }
            }
            else
            {
                response += character;
            }

            isEscaped = false;
        }
        else if (character == '\\')
        {
            isEscaped = true;
        }
        else if (character == '"')
        {
            break;
        }
        else
        {
            response += character;
        }
    }

    return response;
}

bool checkOllamaStatus()
{
    const std::string output = executeCommandAndCaptureOutput("curl -s \"http://localhost:11434/api/tags\" 2>&1");
    return output.find("\"models\"") != std::string::npos;
}

std::string generateOllamaStatusMessage()
{
    if (checkOllamaStatus())
    {
        return "Ollama server is running. Normal chat messages are sent to Ollama automatically.";
    }

    return "Ollama server is not running. Normal chat messages will use local rule-based responses. Start Ollama with 'ollama serve' or open the Ollama app.";
}

bool sendPromptToOllama(const std::string& prompt)
{
    const std::string defaultModelName = "gemma4";
    const std::string command = buildOllamaGenerateCommand(prompt, defaultModelName);
    const std::string jsonText = executeCommandAndCaptureOutput(command);
    const std::string response = extractResponseField(jsonText);

    return !response.empty();
}

std::string askOllama(const std::string& prompt, const std::string& modelName)
{
    const std::string command = buildOllamaGenerateCommand(prompt, modelName);
    const std::string jsonText = executeCommandAndCaptureOutput(command);

    if (jsonText.empty())
    {
        return "Ollama request failed. Check that curl is available and Ollama is running at http://localhost:11434.";
    }

    const std::string response = extractResponseField(jsonText);

    if (response.empty())
    {
        return "Ollama response could not be read. Check that Ollama is running, the model is installed, and the response contains a response field.";
    }

    return response;
}
