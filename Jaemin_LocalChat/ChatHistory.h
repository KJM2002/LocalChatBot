#pragma once

#include <string>
#include <vector>

struct Message
{
    std::string sender;
    std::string text;
};

void addMessage(std::vector<Message>& history, const std::string& sender, const std::string& text);
void clearChatHistory(std::vector<Message>& history);
std::vector<Message> searchMessages(const std::vector<Message>& history, const std::string& keyword);
std::string buildConversationPrompt(const std::vector<Message>& history);
