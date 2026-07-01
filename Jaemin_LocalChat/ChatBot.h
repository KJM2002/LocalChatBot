#pragma once

#include <string>
#include <vector>

#include "UserProfile.h"

struct KeywordResponse
{
    std::vector<std::string> keywords;
    std::string response;
};

std::string generateResponse(const std::string& input, UserProfile& userProfile);
bool shouldUseLocalChatResponse(const std::string& input);
bool isExitCommand(const std::string& input);
bool isHistoryCommand(const std::string& input);
bool isClearHistoryCommand(const std::string& input);
bool isSaveCommand(const std::string& input);
bool isLoadCommand(const std::string& input);
bool startsWith(const std::string& text, const std::string& prefix);
std::string extractSearchKeyword(const std::string& input);
int calculateKeywordScore(const std::string& input, const std::vector<std::string>& keywords);
std::string generateKeywordResponse(const std::string& input);
std::vector<KeywordResponse> createKeywordResponses();
std::string generateNameMemoryResponse(const std::string& input, UserProfile& userProfile);
