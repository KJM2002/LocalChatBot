#include "FileManager.h"

#include <cstdio>
#include <fstream>
#include <iostream>

bool saveChatHistoryToFile(const std::vector<Message>& history, const std::string& fileName)
{
    std::ofstream outputFile(fileName);

    if (!outputFile.is_open())
    {
        return false;
    }

    for (const Message& message : history)
    {
        outputFile << message.sender << ": " << message.text << '\n';
    }

    return true;
}

bool loadChatHistoryFromFile(std::vector<Message>& history, const std::string& fileName)
{
    std::ifstream inputFile(fileName);

    if (!inputFile.is_open())
    {
        return false;
    }

    history.clear();

    std::string line;
    const std::string separator = ": ";

    while (std::getline(inputFile, line))
    {
        const std::string::size_type separatorPosition = line.find(separator);

        if (separatorPosition != std::string::npos)
        {
            const std::string sender = line.substr(0, separatorPosition);
            const std::string text = line.substr(separatorPosition + separator.length());
            addMessage(history, sender, text);
        }
    }

    return true;
}

bool printChatHistoryFromFile(const std::string& fileName)
{
    std::ifstream inputFile(fileName);

    if (!inputFile.is_open())
    {
        return false;
    }

    std::cout << "----- Saved Chat History -----\n";

    std::string line;
    bool hasAnyLine = false;

    while (std::getline(inputFile, line))
    {
        std::cout << line << '\n';
        hasAnyLine = true;
    }

    if (!hasAnyLine)
    {
        std::cout << "No saved messages yet.\n";
    }

    std::cout << "---------------------------------------------------\n";

    return true;
}

bool deleteChatHistoryFile(const std::string& fileName)
{
    if (!doesFileExist(fileName))
    {
        return true;
    }

    return std::remove(fileName.c_str()) == 0;
}

bool doesFileExist(const std::string& fileName)
{
    std::ifstream inputFile(fileName);
    return inputFile.is_open();
}

bool saveUserProfileToFile(const UserProfile& userProfile, const std::string& fileName)
{
    std::ofstream outputFile(fileName);

    if (!outputFile.is_open())
    {
        return false;
    }

    outputFile << "name=" << userProfile.name << '\n';
    outputFile << "hasName=" << (userProfile.hasName ? "1" : "0") << '\n';

    return true;
}

bool loadUserProfileFromFile(UserProfile& userProfile, const std::string& fileName)
{
    std::ifstream inputFile(fileName);

    if (!inputFile.is_open())
    {
        return false;
    }

    UserProfile loadedProfile = createDefaultUserProfile();

    std::string line;
    while (std::getline(inputFile, line))
    {
        const std::string namePrefix = "name=";
        const std::string hasNamePrefix = "hasName=";

        if (line.substr(0, namePrefix.length()) == namePrefix)
        {
            loadedProfile.name = line.substr(namePrefix.length());
        }
        else if (line.substr(0, hasNamePrefix.length()) == hasNamePrefix)
        {
            loadedProfile.hasName = line.substr(hasNamePrefix.length()) == "1";
        }
    }

    if (!loadedProfile.hasName)
    {
        loadedProfile.name = "";
    }

    userProfile = loadedProfile;
    return true;
}
