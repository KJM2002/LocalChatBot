#include "ChatBot.h"

#include <vector>

struct ResponseRule
{
    std::string input;
    std::string response;
};

std::vector<KeywordResponse> createKeywordResponses()
{
    return
    {
        {
            {"hello", "hi", "안녕", "반가워"},
            "Hello. Nice to meet you."
        },
        {
            {"name", "이름", "누구"},
            "My name is Local C++ ChatBot."
        },
        {
            {"cpp", "C++", "c++", "프로그래밍", "언어"},
            "C++ learning starts with basic syntax, functions, arrays, and simple projects."
        },
        {
            {"study", "공부", "학습", "배우기"},
            "This project helps you study C++ step by step."
        },
        {
            {"vector", "struct", "자료구조", "배열"},
            "Data structures help you organize and search data efficiently."
        },
        {
            {"file", "save", "저장", "기록"},
            "File I/O allows the program to keep data even after it exits."
        },
        {
            {"search", "검색", "찾기"},
            "Search uses a simple linear scan to find matching messages."
        }
    };
}

int calculateKeywordScore(const std::string& input, const std::vector<std::string>& keywords)
{
    int score = 0;

    for (const std::string& keyword : keywords)
    {
        if (input.find(keyword) != std::string::npos)
        {
            score++;
        }
    }

    return score;
}

std::string generateKeywordResponse(const std::string& input)
{
    const std::vector<KeywordResponse> keywordResponses = createKeywordResponses();

    int bestScore = 0;
    std::string bestResponse = "I do not understand yet. Try typing 'help'.";

    for (const KeywordResponse& keywordResponse : keywordResponses)
    {
        const int score = calculateKeywordScore(input, keywordResponse.keywords);

        if (score > bestScore)
        {
            bestScore = score;
            bestResponse = keywordResponse.response;
        }
    }

    return bestResponse;
}

std::string generateNameMemoryResponse(const std::string& input, UserProfile& userProfile)
{
    if (isSetNameCommand(input))
    {
        const std::string name = extractNameFromInput(input);

        if (name.empty())
        {
            return "Please tell me your name after 'my name is'.";
        }

        setUserName(userProfile, name);

        if (startsWith(input, "내 이름은 "))
        {
            return name + "님, 이름을 기억할게요.";
        }

        return "I will remember your name, " + name + ".";
    }

    if (isAskNameCommand(input))
    {
        if (!hasUserName(userProfile))
        {
            if (input == "내 이름 뭐야?")
            {
                return "아직 이름을 모릅니다.";
            }

            return "I do not know your name yet.";
        }

        if (input == "내 이름 뭐야?")
        {
            return "당신의 이름은 " + getUserName(userProfile) + "입니다.";
        }

        return "Your name is " + getUserName(userProfile) + ".";
    }

    return "";
}

std::string generateResponse(const std::string& input, UserProfile& userProfile)
{
    const std::string nameMemoryResponse = generateNameMemoryResponse(input, userProfile);

    if (!nameMemoryResponse.empty())
    {
        return nameMemoryResponse;
    }

    const std::vector<ResponseRule> rules =
    {
        {"hello", "Hello. I am a local C++ chatbot."},
        {"hi", "Hi. Nice to meet you."},
        {"안녕", "안녕하세요. 저는 C++로 만든 로컬 챗봇입니다."},
        {
            "help",
            "사용 가능한 명령어:\n"
            "hello / hi / 안녕 : 간단한 인사 응답을 출력합니다.\n"
            "help : 이 명령어 도움말을 출력합니다.\n"
            "name : 챗봇의 이름을 출력합니다.\n"
            "study : 이 C++ 학습 프로젝트의 목적을 설명합니다.\n"
            "cpp : C++ 학습과 관련된 짧은 설명을 출력합니다.\n"
            "history : 현재 메모리에 저장된 대화 기록을 출력합니다.\n"
            "clearhistory : 메모리 대화 기록과 저장 파일을 삭제하고 콘솔 화면을 비웁니다.\n"
            "deletehistory / clearchat : clearhistory와 같은 기능입니다.\n"
            "save : 현재 대화 기록을 chat_history.txt에 저장합니다.\n"
            "load : chat_history.txt에 저장된 대화 기록을 출력합니다.\n"
            "search keyword : 현재 대화 기록에서 keyword를 검색합니다.\n"
            "profile : 저장된 사용자 프로필 정보를 출력합니다.\n"
            "clearprofile : 저장된 사용자 프로필 정보를 삭제합니다.\n"
            "aistatus : Ollama 서버가 실행 중인지 확인합니다.\n"
            "aimodel : 현재 사용 중인 Ollama 모델 이름을 출력합니다.\n"
            "setaimodel modelName : 사용할 Ollama 모델을 변경합니다.\n"
            "ai prompt : prompt 내용을 Ollama에 명시적으로 보냅니다.\n"
            "일반 문장 : Ollama 서버가 실행 중이면 AI와 바로 대화합니다.\n"
            "my name is Jaemin : 사용자 이름을 프로필에 저장합니다.\n"
            "what is my name? : 저장된 사용자 이름을 확인합니다.\n"
            "exit / quit : 데이터를 저장하고 프로그램을 종료합니다."
        },
        {"name", "My name is Local C++ ChatBot."},
        {"study", "This project helps you study C++ step by step."},
        {"cpp", "C++ is a powerful language for learning memory, structure, and algorithms."}
    };

    for (const ResponseRule& rule : rules)
    {
        if (input == rule.input)
        {
            return rule.response;
        }
    }

    return generateKeywordResponse(input);
}

bool shouldUseLocalChatResponse(const std::string& input)
{
    return input.empty()
        || input == "hello"
        || input == "hi"
        || input == "안녕"
        || input == "help"
        || input == "name"
        || input == "study"
        || input == "cpp"
        || isSetNameCommand(input)
        || isAskNameCommand(input);
}

bool isExitCommand(const std::string& input)
{
    return input == "exit" || input == "quit";
}

bool isHistoryCommand(const std::string& input)
{
    return input == "history";
}

bool isClearHistoryCommand(const std::string& input)
{
    return input == "clearhistory" || input == "deletehistory" || input == "clearchat";
}

bool isSaveCommand(const std::string& input)
{
    return input == "save";
}

bool isLoadCommand(const std::string& input)
{
    return input == "load";
}

bool startsWith(const std::string& text, const std::string& prefix)
{
    if (prefix.length() > text.length())
    {
        return false;
    }

    return text.substr(0, prefix.length()) == prefix;
}

std::string extractSearchKeyword(const std::string& input)
{
    const std::string searchPrefix = "search ";

    if (!startsWith(input, searchPrefix))
    {
        return "";
    }

    return input.substr(searchPrefix.length());
}
