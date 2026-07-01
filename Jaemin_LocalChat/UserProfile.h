#pragma once

#include <string>

struct UserProfile
{
    std::string name;
    bool hasName;
};

UserProfile createDefaultUserProfile();
bool hasUserName(const UserProfile& userProfile);
void setUserName(UserProfile& userProfile, const std::string& name);
std::string getUserName(const UserProfile& userProfile);
bool isSetNameCommand(const std::string& input);
bool isAskNameCommand(const std::string& input);
std::string extractNameFromInput(const std::string& input);
void clearUserProfile(UserProfile& userProfile);
std::string generateProfileText(const UserProfile& userProfile);
bool isProfileCommand(const std::string& input);
bool isClearProfileCommand(const std::string& input);
