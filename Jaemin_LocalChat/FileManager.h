#pragma once

#include <string>
#include <vector>

#include "ChatHistory.h"
#include "UserProfile.h"

bool saveChatHistoryToFile(const std::vector<Message>& history, const std::string& fileName);
bool loadChatHistoryFromFile(std::vector<Message>& history, const std::string& fileName);
bool printChatHistoryFromFile(const std::string& fileName);
bool deleteChatHistoryFile(const std::string& fileName);
bool doesFileExist(const std::string& fileName);
bool saveUserProfileToFile(const UserProfile& userProfile, const std::string& fileName);
bool loadUserProfileFromFile(UserProfile& userProfile, const std::string& fileName);
