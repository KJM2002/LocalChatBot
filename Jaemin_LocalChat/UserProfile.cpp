#include "UserProfile.h"

std::string trimSpaces(const std::string& text)
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

bool hasPrefix(const std::string& text, const std::string& prefix)
{
    if (prefix.length() > text.length())
    {
        return false;
    }

    return text.substr(0, prefix.length()) == prefix;
}

UserProfile createDefaultUserProfile()
{
    UserProfile userProfile;
    userProfile.name = "";
    userProfile.hasName = false;

    return userProfile;
}

bool hasUserName(const UserProfile& userProfile)
{
    return userProfile.hasName;
}

void setUserName(UserProfile& userProfile, const std::string& name)
{
    userProfile.name = name;
    userProfile.hasName = true;
}

std::string getUserName(const UserProfile& userProfile)
{
    return userProfile.name;
}

bool isSetNameCommand(const std::string& input)
{
    return hasPrefix(input, "my name is ") || hasPrefix(input, "내 이름은 ");
}

bool isAskNameCommand(const std::string& input)
{
    return input == "what is my name?" || input == "내 이름 뭐야?";
}

std::string extractNameFromInput(const std::string& input)
{
    const std::string englishPrefix = "my name is ";
    const std::string koreanPrefix = "내 이름은 ";

    if (hasPrefix(input, englishPrefix))
    {
        return trimSpaces(input.substr(englishPrefix.length()));
    }

    if (hasPrefix(input, koreanPrefix))
    {
        return trimSpaces(input.substr(koreanPrefix.length()));
    }

    return "";
}

void clearUserProfile(UserProfile& userProfile)
{
    userProfile.name = "";
    userProfile.hasName = false;
}

std::string generateProfileText(const UserProfile& userProfile)
{
    if (hasUserName(userProfile))
    {
        return "Current profile: name = " + getUserName(userProfile);
    }

    return "Current profile: name is not set.";
}

bool isProfileCommand(const std::string& input)
{
    return input == "profile";
}

bool isClearProfileCommand(const std::string& input)
{
    return input == "clearprofile";
}
