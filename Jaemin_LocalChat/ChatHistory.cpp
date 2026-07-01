#include "ChatHistory.h"

std::size_t getSafeUtf8PrefixLength(const std::string& text, std::size_t maxLength)
{
    if (text.length() <= maxLength)
    {
        return text.length();
    }

    std::size_t index = 0;

    while (index < text.length())
    {
        const unsigned char firstByte = static_cast<unsigned char>(text[index]);
        std::size_t characterLength = 1;

        if ((firstByte & 0xE0) == 0xC0)
        {
            characterLength = 2;
        }
        else if ((firstByte & 0xF0) == 0xE0)
        {
            characterLength = 3;
        }
        else if ((firstByte & 0xF8) == 0xF0)
        {
            characterLength = 4;
        }

        if (index + characterLength > maxLength)
        {
            break;
        }

        index += characterLength;
    }

    return index;
}

std::string shortenMemoryText(const std::string& text)
{
    const std::size_t maxLength = 600;

    if (text.length() <= maxLength)
    {
        return text;
    }

    return text.substr(0, getSafeUtf8PrefixLength(text, maxLength)) + "...";
}

void addMessage(std::vector<Message>& history, const std::string& sender, const std::string& text)
{
    Message message;
    message.sender = sender;
    message.text = text;

    history.push_back(message);
}

void clearChatHistory(std::vector<Message>& history)
{
    history.clear();
}

std::vector<Message> searchMessages(const std::vector<Message>& history, const std::string& keyword)
{
    std::vector<Message> results;

    for (const Message& message : history)
    {
        if (message.text.find(keyword) != std::string::npos)
        {
            results.push_back(message);
        }
    }

    return results;
}

std::string buildConversationPrompt(const std::vector<Message>& history)
{
    const std::size_t maxMemoryMessages = 12;
    const std::size_t startIndex = history.size() > maxMemoryMessages ? history.size() - maxMemoryMessages : 0;

    std::string prompt;
    prompt += "You are a helpful local AI chatbot in a C++ console learning project.\n";
    prompt += "Use the recent conversation memory below to keep the conversation connected.\n";
    prompt += "Answer the latest user message naturally.\n\n";
    prompt += "Recent conversation memory:\n";

    for (std::size_t index = startIndex; index < history.size(); index++)
    {
        prompt += history[index].sender;
        prompt += ": ";
        prompt += shortenMemoryText(history[index].text);
        prompt += "\n";
    }

    prompt += "\nBot:";

    return prompt;
}
