#pragma once

#include <string>

bool isAICommand(const std::string& input);
bool isAIStatusCommand(const std::string& input);
bool isAIModelCommand(const std::string& input);
bool isSetAIModelCommand(const std::string& input);
std::string extractAIPrompt(const std::string& input);
std::string extractAIModelName(const std::string& input);
bool checkOllamaStatus();
std::string generateOllamaStatusMessage();
bool sendPromptToOllama(const std::string& prompt);
std::string executeCommandAndCaptureOutput(const std::string& command);
std::string escapeJsonString(const std::string& text);
std::string buildOllamaGenerateCommand(const std::string& prompt, const std::string& modelName);
std::string extractResponseField(const std::string& jsonText);
std::string askOllama(const std::string& prompt, const std::string& modelName);
