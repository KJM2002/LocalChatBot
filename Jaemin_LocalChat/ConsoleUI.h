#pragma once

#include <atomic>
#include <vector>
#include <string>

#include "ChatHistory.h"

void setupConsoleEncoding();
void printWelcomeMessage();
void printUserPrompt();
void printBotMessage(const std::string& message);
void printBotMessageInstant(const std::string& message);
void clearConsoleScreen();
void runThinkingAnimation(const std::atomic<bool>& isThinking);
void clearThinkingMessage();
void printChatHistory(const std::vector<Message>& history);
void printSearchResults(const std::vector<Message>& results, const std::string& keyword);
