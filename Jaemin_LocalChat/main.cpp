#include <iostream>
#include <atomic>
#include <functional>
#include <string>
#include <thread>
#include <vector>

#include "ChatBot.h"
#include "ChatHistory.h"
#include "ConsoleUI.h"
#include "FileManager.h"
#include "LocalAIConnector.h"
#include "UserProfile.h"

std::string askOllamaWithThinking(const std::string& prompt, const std::string& modelName)
{
    std::atomic<bool> isThinking(true);
    std::thread thinkingThread(runThinkingAnimation, std::ref(isThinking));

    const std::string response = askOllama(prompt, modelName);

    isThinking.store(false);

    if (thinkingThread.joinable())
    {
        thinkingThread.join();
    }

    clearThinkingMessage();

    return response;
}

void saveDataBeforeExit(
    const std::vector<Message>& history,
    const std::string& chatHistoryFileName,
    const UserProfile& userProfile,
    const std::string& profileFileName)
{
    if (history.empty())
    {
        if (deleteChatHistoryFile(chatHistoryFileName))
        {
            printBotMessageInstant("No chat history to save.");
        }
        else
        {
            printBotMessageInstant("Failed to clear empty chat history file.");
        }
    }
    else if (saveChatHistoryToFile(history, chatHistoryFileName))
    {
        printBotMessageInstant("Chat history saved successfully.");
    }
    else
    {
        printBotMessageInstant("Failed to save chat history.");
    }

    if (saveUserProfileToFile(userProfile, profileFileName))
    {
        printBotMessageInstant("Profile saved successfully.");
    }
    else
    {
        printBotMessageInstant("Failed to save profile.");
    }
}

int main()
{
    const std::string chatHistoryFileName = "chat_history.txt";
    const std::string profileFileName = "profile.txt";
    std::string currentAIModel = "gemma4";

    setupConsoleEncoding();
    printWelcomeMessage();

    std::vector<Message> history;
    UserProfile userProfile = createDefaultUserProfile();

    if (doesFileExist(profileFileName))
    {
        if (loadUserProfileFromFile(userProfile, profileFileName))
        {
            printBotMessageInstant("Profile loaded successfully.");

            if (hasUserName(userProfile))
            {
                printBotMessageInstant("Welcome back, " + getUserName(userProfile) + ".");
            }
        }
        else
        {
            printBotMessageInstant("Could not load saved profile.");
        }
    }

    if (doesFileExist(chatHistoryFileName))
    {
        loadChatHistoryFromFile(history, chatHistoryFileName);
        std::cout << "\nPrevious chat history found:\n";
        if (!printChatHistoryFromFile(chatHistoryFileName))
        {
            printBotMessageInstant("Could not load previous chat history.");
        }
    }
    else
    {
        printBotMessageInstant("No previous chat history found.");
    }

    std::string input;

    while (true)
    {
        printUserPrompt();
        if (!std::getline(std::cin, input))
        {
            printBotMessageInstant("Program terminated.");
            saveDataBeforeExit(history, chatHistoryFileName, userProfile, profileFileName);
            break;
        }

        if (isExitCommand(input))
        {
            printBotMessageInstant("Program terminated.");
            saveDataBeforeExit(history, chatHistoryFileName, userProfile, profileFileName);
            break;
        }

        if (isClearHistoryCommand(input))
        {
            // Clearhistory removes both the current memory history and the saved chat_history.txt file.
            clearChatHistory(history);
            const bool wasDeleted = deleteChatHistoryFile(chatHistoryFileName);

            clearConsoleScreen();

            if (wasDeleted)
            {
                printBotMessageInstant("대화 기록이 모두 삭제되었습니다.");
            }
            else
            {
                printBotMessageInstant("메모리 기록은 삭제했지만 저장 파일은 삭제하지 못했습니다.");
            }

            continue;
        }

        if (isHistoryCommand(input))
        {
            // History is a view command, so it is not saved as a chat message.
            printChatHistory(history);
            continue;
        }

        if (isSaveCommand(input))
        {
            // Save is a file command, so it is not saved as a chat message.
            if (saveChatHistoryToFile(history, chatHistoryFileName))
            {
                printBotMessageInstant("Chat history saved successfully.");
            }
            else
            {
                printBotMessageInstant("Failed to save chat history.");
            }
            continue;
        }

        if (isLoadCommand(input))
        {
            // Load shows saved file contents, so it is not saved as a chat message.
            if (!printChatHistoryFromFile(chatHistoryFileName))
            {
                printBotMessageInstant("No saved chat history found.");
            }
            continue;
        }

        if (input == "search" || startsWith(input, "search "))
        {
            // Search is a lookup command, so it is not saved as a chat message.
            const std::string keyword = extractSearchKeyword(input);

            if (keyword.empty())
            {
                printBotMessageInstant("Usage: search keyword");
            }
            else
            {
                const std::vector<Message> results = searchMessages(history, keyword);
                printSearchResults(results, keyword);
            }

            continue;
        }

        if (isProfileCommand(input))
        {
            // Profile is a status command, so it is not saved as a chat message.
            printBotMessageInstant(generateProfileText(userProfile));
            continue;
        }

        if (isClearProfileCommand(input))
        {
            // Clearprofile changes profile state, so it is saved to profile.txt immediately.
            clearUserProfile(userProfile);
            if (saveUserProfileToFile(userProfile, profileFileName))
            {
                printBotMessageInstant("Profile cleared. I no longer know your name.");
            }
            else
            {
                printBotMessageInstant("Profile cleared, but failed to save profile.");
            }
            continue;
        }

        if (isAIStatusCommand(input))
        {
            printBotMessageInstant(generateOllamaStatusMessage());
            continue;
        }

        if (isAIModelCommand(input))
        {
            printBotMessageInstant("Current AI model: " + currentAIModel);
            continue;
        }

        if (isSetAIModelCommand(input))
        {
            const std::string modelName = extractAIModelName(input);

            if (modelName.empty())
            {
                printBotMessageInstant("Usage: setaimodel modelName");
            }
            else
            {
                currentAIModel = modelName;
                printBotMessageInstant("AI model changed to: " + currentAIModel);
            }
            continue;
        }

        if (isAICommand(input))
        {
            const std::string aiPrompt = extractAIPrompt(input);

            if (aiPrompt.empty())
            {
                printBotMessageInstant("Usage: ai prompt");
            }
            else
            {
                addMessage(history, "User", aiPrompt);

                const std::string memoryPrompt = buildConversationPrompt(history);
                const std::string aiResponse = askOllamaWithThinking(memoryPrompt, currentAIModel);
                addMessage(history, "Bot", aiResponse);

                printBotMessage(aiResponse);
            }
            continue;
        }

        addMessage(history, "User", input);

        std::string response;
        bool shouldPrintInstantly = false;

        if (shouldUseLocalChatResponse(input))
        {
            response = generateResponse(input, userProfile);
            shouldPrintInstantly = true;
        }
        else if (checkOllamaStatus())
        {
            const std::string memoryPrompt = buildConversationPrompt(history);
            response = askOllamaWithThinking(memoryPrompt, currentAIModel);
        }
        else
        {
            response = generateResponse(input, userProfile);
        }

        addMessage(history, "Bot", response);

        if (shouldPrintInstantly)
        {
            printBotMessageInstant(response);
        }
        else
        {
            printBotMessage(response);
        }

        if (isSetNameCommand(input))
        {
            if (saveUserProfileToFile(userProfile, profileFileName))
            {
                printBotMessageInstant("Profile saved successfully.");
            }
            else
            {
                printBotMessageInstant("Failed to save profile.");
            }
        }
    }

    return 0;
}
