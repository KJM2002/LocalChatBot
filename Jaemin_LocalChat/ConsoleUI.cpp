#include "ConsoleUI.h"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

void setupConsoleEncoding()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

void setConsoleCursorVisible(bool isVisible)
{
#ifdef _WIN32
    HANDLE consoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    if (consoleOutput == INVALID_HANDLE_VALUE || consoleOutput == nullptr)
    {
        return;
    }

    CONSOLE_CURSOR_INFO cursorInfo;

    if (!GetConsoleCursorInfo(consoleOutput, &cursorInfo))
    {
        return;
    }

    cursorInfo.bVisible = isVisible ? TRUE : FALSE;
    SetConsoleCursorInfo(consoleOutput, &cursorInfo);
#endif
}

void printWelcomeMessage()
{
    std::cout << "Local C++ ChatBot started.\n";
    std::cout << "Type 'help' to see commands.\n";
    std::cout << "Type 'aistatus' to check the Ollama server.\n";
    std::cout << "Type 'exit' or 'quit' to stop.\n";
}

void printUserPrompt()
{
    std::cout << "\nYou: ";
}

std::size_t getUtf8CharacterLength(unsigned char firstByte)
{
    if ((firstByte & 0x80) == 0)
    {
        return 1;
    }

    if ((firstByte & 0xE0) == 0xC0)
    {
        return 2;
    }

    if ((firstByte & 0xF0) == 0xE0)
    {
        return 3;
    }

    if ((firstByte & 0xF8) == 0xF0)
    {
        return 4;
    }

    return 1;
}

void printBotMessage(const std::string& message)
{
    const int typingDelayMilliseconds = 12;

    std::cout << "Bot: " << std::flush;

    for (std::size_t index = 0; index < message.length();)
    {
        const std::size_t characterLength = getUtf8CharacterLength(static_cast<unsigned char>(message[index]));
        const std::size_t remainingLength = message.length() - index;
        const std::size_t outputLength = characterLength <= remainingLength ? characterLength : remainingLength;

        std::cout << message.substr(index, outputLength) << std::flush;

        index += outputLength;

        if (message[index - outputLength] != '\n')
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(typingDelayMilliseconds));
        }
    }

    std::cout << '\n';
}

void printBotMessageInstant(const std::string& message)
{
    std::cout << "Bot: " << message << '\n';
}

void clearConsoleScreen()
{
#ifdef _WIN32
    HANDLE consoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    if (consoleOutput == INVALID_HANDLE_VALUE || consoleOutput == nullptr)
    {
        return;
    }

    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;

    if (!GetConsoleScreenBufferInfo(consoleOutput, &screenBufferInfo))
    {
        std::cout << "\x1B[2J\x1B[H" << std::flush;
        return;
    }

    const DWORD cellCount = static_cast<DWORD>(screenBufferInfo.dwSize.X) * static_cast<DWORD>(screenBufferInfo.dwSize.Y);
    const COORD homePosition = {0, 0};
    DWORD writtenCount = 0;

    FillConsoleOutputCharacterA(consoleOutput, ' ', cellCount, homePosition, &writtenCount);
    FillConsoleOutputAttribute(consoleOutput, screenBufferInfo.wAttributes, cellCount, homePosition, &writtenCount);
    SetConsoleCursorPosition(consoleOutput, homePosition);
#else
    std::cout << "\x1B[2J\x1B[3J\x1B[H" << std::flush;
#endif
}

void runThinkingAnimation(const std::atomic<bool>& isThinking)
{
    const std::string thinkingText = "생각 중...";
    const std::string clearSuffix = "            ";
    std::vector<std::string> characters;

    for (std::size_t index = 0; index < thinkingText.length();)
    {
        const std::size_t characterLength = getUtf8CharacterLength(static_cast<unsigned char>(thinkingText[index]));
        const std::size_t remainingLength = thinkingText.length() - index;
        const std::size_t outputLength = characterLength <= remainingLength ? characterLength : remainingLength;

        characters.push_back(thinkingText.substr(index, outputLength));
        index += outputLength;
    }

    std::size_t visibleCount = 1;
    setConsoleCursorVisible(false);

    while (isThinking.load())
    {
        std::cout << "\rBot: ";

        for (std::size_t index = 0; index < visibleCount && index < characters.size(); index++)
        {
            std::cout << characters[index];
        }

        std::cout << clearSuffix << "\r" << std::flush;

        visibleCount++;

        if (visibleCount > characters.size())
        {
            visibleCount = 1;
            std::this_thread::sleep_for(std::chrono::milliseconds(350));
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(130));
        }
    }
}

void clearThinkingMessage()
{
    std::cout << "\r" << std::string(80, ' ') << "\r" << std::flush;
    setConsoleCursorVisible(true);
}

void printChatHistory(const std::vector<Message>& history)
{
    std::cout << "----- Chat History -----\n";

    if (history.empty())
    {
        std::cout << "No messages yet.\n";
    }
    else
    {
        for (const Message& message : history)
        {
            std::cout << message.sender << ": " << message.text << '\n';
        }
    }

    std::cout << "-------------------------------------------------------------------------------\n";
}

void printSearchResults(const std::vector<Message>& results, const std::string& keyword)
{
    std::cout << "----- Search Results -----\n";

    if (results.empty())
    {
        std::cout << "No messages found containing: " << keyword << '\n';
    }
    else
    {
        for (const Message& message : results)
        {
            std::cout << message.sender << ": " << message.text << '\n';
        }
    }

    std::cout << "Total results: " << results.size() << '\n';
    std::cout << "----------------\n";
}
