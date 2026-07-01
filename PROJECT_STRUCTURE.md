# Jaemin_LocalChat 프로젝트 구조 정리

이 문서는 현재 `Jaemin_LocalChat` 프로젝트가 어떤 파일 구조로 나뉘어 있는지, 각 파일이 어떤 역할을 하는지, 그리고 로컬 Ollama AI 모델을 어떻게 연결했는지 정리한 문서입니다.

## 프로젝트 개요

`Jaemin_LocalChat`은 C++ 학습을 위해 단계적으로 확장한 로컬 콘솔 채팅 프로그램입니다.

현재 프로젝트는 단순 규칙 기반 챗봇에서 시작해 다음 기능까지 확장되어 있습니다.

- C++17 콘솔 프로그램
- Visual Studio 솔루션/프로젝트 사용
- 역할별 `.h` / `.cpp` 파일 분리
- 규칙 기반 응답
- 대화 기록 메모리 저장
- 대화 기록 파일 저장/불러오기/삭제
- 사용자 프로필 저장
- Ollama 로컬 AI 모델 연결
- 기본 AI 모델: `gemma4`
- 일반 문장 입력 시 `ai` 접두어 없이 Ollama와 대화
- Ollama 응답 대기 중 `생각 중...` 애니메이션 출력
- AI 답변은 타자 효과로 출력
- 명령어 응답은 즉시 출력

## 전체 폴더 구조

핵심 구조는 다음과 같습니다.

```text
Jaemin_LocalChat/
├─ Jaemin_LocalChat.sln
├─ PROJECT_STRUCTURE.md
├─ Jaemin_LocalChat/
│  ├─ main.cpp
│  ├─ ChatBot.h
│  ├─ ChatBot.cpp
│  ├─ ChatHistory.h
│  ├─ ChatHistory.cpp
│  ├─ ConsoleUI.h
│  ├─ ConsoleUI.cpp
│  ├─ FileManager.h
│  ├─ FileManager.cpp
│  ├─ LocalAIConnector.h
│  ├─ LocalAIConnector.cpp
│  ├─ UserProfile.h
│  ├─ UserProfile.cpp
│  ├─ Jaemin_LocalChat.vcxproj
│  ├─ Jaemin_LocalChat.vcxproj.filters
│  └─ Jaemin_LocalChat.vcxproj.user
└─ x64/
   └─ Debug/
      └─ Jaemin_LocalChat.exe
```

`x64/Debug` 폴더는 빌드 결과물이 생성되는 위치입니다. 직접 수정해야 하는 코드는 대부분 `Jaemin_LocalChat/Jaemin_LocalChat/` 폴더 안에 있습니다.

## 파일별 역할

### `main.cpp`

프로그램의 전체 실행 흐름을 담당합니다.

주요 역할:

- 콘솔 인코딩 설정
- 시작 메시지 출력
- 저장된 프로필과 대화 기록 불러오기
- 사용자 입력 반복 처리
- `exit`, `help`, `history`, `clearhistory`, `aistatus` 같은 명령어 분기
- 일반 입력을 Ollama AI 또는 로컬 규칙 응답으로 전달
- 프로그램 종료 시 대화 기록과 프로필 저장
- Ollama 응답 대기 중 `생각 중...` 애니메이션 실행

중요한 흐름:

```text
사용자 입력
→ 명령어인지 확인
→ 명령어면 해당 기능 실행
→ 일반 문장이면 Ollama 서버 상태 확인
→ Ollama 실행 중이면 AI 응답 생성
→ Ollama가 꺼져 있으면 로컬 규칙 기반 응답 사용
→ 응답을 대화 기록에 저장
```

### `ChatBot.h` / `ChatBot.cpp`

로컬 규칙 기반 챗봇 로직을 담당합니다.

주요 역할:

- `hello`, `hi`, `안녕`, `help`, `name`, `study`, `cpp` 같은 로컬 응답 처리
- `exit`, `history`, `clearhistory`, `save`, `load` 같은 명령어 판별
- 키워드 기반 응답 생성
- 사용자 이름 저장/조회 명령 처리
- AI로 보내지 않고 로컬에서 처리해야 하는 입력 판별

예시:

```cpp
bool isExitCommand(const std::string& input);
bool isClearHistoryCommand(const std::string& input);
std::string generateResponse(const std::string& input, UserProfile& userProfile);
bool shouldUseLocalChatResponse(const std::string& input);
```

`help` 명령어 설명도 이 파일의 로컬 응답 규칙 안에 들어 있습니다.

### `ChatHistory.h` / `ChatHistory.cpp`

대화 기록을 메모리에서 관리합니다.

주요 역할:

- `Message` 구조체 정의
- 사용자/봇 메시지를 `std::vector<Message>`에 추가
- 대화 기록 검색
- 대화 기록 삭제
- Ollama에 보낼 최근 대화 문맥 프롬프트 생성

대화 기록 구조:

```cpp
struct Message
{
    std::string sender;
    std::string text;
};
```

AI 대화 흐름을 이어가기 위해 `buildConversationPrompt()` 함수가 최근 대화 기록을 하나의 프롬프트로 묶습니다.

현재는 최근 12개 메시지를 Ollama에 전달합니다. 너무 긴 메시지는 잘라서 프롬프트가 과도하게 길어지지 않도록 했습니다.

### `ConsoleUI.h` / `ConsoleUI.cpp`

콘솔 출력과 화면 표시를 담당합니다.

주요 역할:

- UTF-8 콘솔 인코딩 설정
- 시작 메시지 출력
- 사용자 입력 프롬프트 출력
- 봇 메시지 출력
- AI 답변 타자 효과 출력
- 명령어 응답 즉시 출력
- `생각 중...` 애니메이션 출력
- 콘솔 화면 지우기
- 대화 기록과 검색 결과 출력

출력 함수는 두 종류로 나뉩니다.

```cpp
void printBotMessage(const std::string& message);
void printBotMessageInstant(const std::string& message);
```

- `printBotMessage`: AI 답변처럼 한 글자씩 출력합니다.
- `printBotMessageInstant`: `help`, `save`, `aistatus` 같은 명령어 결과를 한 번에 출력합니다.

`clearhistory` 명령은 `clearConsoleScreen()`을 호출해 실제 cmd 창에 출력된 이전 대화 내용을 지웁니다.

### `FileManager.h` / `FileManager.cpp`

파일 저장과 불러오기를 담당합니다.

주요 역할:

- 대화 기록을 `chat_history.txt`에 저장
- 저장된 대화 기록 불러오기
- 저장된 대화 기록 출력
- 저장된 대화 기록 파일 삭제
- 사용자 프로필을 `profile.txt`에 저장
- 사용자 프로필 불러오기
- 파일 존재 여부 확인

주요 파일:

```text
chat_history.txt : 저장된 대화 기록
profile.txt      : 저장된 사용자 프로필
```

`clearhistory` 명령을 실행하면 메모리의 대화 기록뿐 아니라 `chat_history.txt`도 삭제합니다.

### `UserProfile.h` / `UserProfile.cpp`

사용자 프로필 정보를 관리합니다.

주요 역할:

- 사용자 이름 저장
- 사용자 이름 조회
- 프로필 초기화
- `profile`, `clearprofile` 명령 처리
- `my name is Jaemin` 같은 입력에서 이름 추출

프로필 구조:

```cpp
struct UserProfile
{
    std::string name;
    bool hasName;
};
```

### `LocalAIConnector.h` / `LocalAIConnector.cpp`

Ollama 로컬 AI 서버와 통신하는 부분입니다.

주요 역할:

- AI 관련 명령어 판별
- 현재 Ollama 서버 상태 확인
- Ollama API 요청 명령 생성
- JSON 문자열 이스케이프
- Ollama 응답 JSON에서 `response` 필드 추출
- 로컬 명령 실행 결과 캡처
- Ollama에 프롬프트 전송

주요 함수:

```cpp
bool checkOllamaStatus();
std::string askOllama(const std::string& prompt, const std::string& modelName);
std::string buildOllamaGenerateCommand(const std::string& prompt, const std::string& modelName);
std::string extractResponseField(const std::string& jsonText);
```

## AI 모델 연결 방식

이 프로젝트는 외부 클라우드 API가 아니라 로컬에서 실행 중인 Ollama 서버에 HTTP 요청을 보내는 방식으로 AI 모델과 연결합니다.

현재 기본 모델은 다음과 같습니다.

```cpp
std::string currentAIModel = "gemma4";
```

Ollama 모델은 로컬에 다운로드되어 있어야 합니다.

```powershell
ollama pull gemma4
```

현재 작업 환경에서는 `ollama` 명령이 PowerShell PATH에 잡히지 않을 수 있어서, 직접 경로로 실행한 적이 있습니다.

```powershell
C:\Users\User\AppData\Local\Programs\Ollama\ollama.exe pull gemma4
```

## Ollama 서버 상태 확인

`aistatus` 명령을 입력하면 Ollama 서버가 켜져 있는지 확인합니다.

내부적으로는 다음 주소를 확인합니다.

```text
http://localhost:11434/api/tags
```

`LocalAIConnector.cpp`의 `checkOllamaStatus()` 함수가 `curl` 명령을 실행하고, 응답에 `"models"`가 포함되어 있는지 확인합니다.

```cpp
bool checkOllamaStatus()
{
    const std::string output = executeCommandAndCaptureOutput("curl -s \"http://localhost:11434/api/tags\" 2>&1");
    return output.find("\"models\"") != std::string::npos;
}
```

## Ollama에 프롬프트 보내는 방식

AI 응답은 Ollama의 generate API를 사용합니다.

요청 주소:

```text
http://localhost:11434/api/generate
```

요청 형태:

```json
{
  "model": "gemma4",
  "prompt": "사용자 입력 또는 대화 문맥",
  "stream": false
}
```

현재 코드는 `curl` 명령을 문자열로 만들고, 그 명령을 실행해서 응답을 받습니다.

```cpp
std::string command = "curl -s -X POST \"http://localhost:11434/api/generate\" ";
command += "-H \"Content-Type: application/json\" ";
command += "-d \"" + jsonBody + "\" 2>&1";
```

`stream`은 `false`로 설정되어 있습니다. 그래서 Ollama 응답 전체가 도착한 뒤 프로그램이 응답 문자열을 추출합니다.

## Windows에서 명령 실행 결과를 가져오는 방식

처음에는 `_popen()` 방식으로 명령 실행 결과를 가져올 수 있지만, Windows Debug 환경에서 파일 핸들 관련 assertion 문제가 발생할 수 있습니다.

그래서 현재 프로젝트는 Windows API 기반으로 명령을 실행합니다.

사용하는 주요 Windows API:

- `CreatePipe`
- `CreateProcessW`
- `ReadFile`
- `WaitForSingleObject`
- `CloseHandle`

이 구조는 `curl` 실행 결과를 프로그램 내부 문자열로 안전하게 받아오기 위해 사용합니다.

## AI 응답 처리 흐름

AI 응답 처리 흐름은 다음과 같습니다.

```text
사용자 입력
→ 메모리 대화 기록에 User 메시지 추가
→ 최근 대화 기록을 프롬프트로 변환
→ Ollama 서버 상태 확인
→ Ollama가 켜져 있으면 /api/generate 호출
→ JSON 응답에서 response 필드 추출
→ 메모리 대화 기록에 Bot 메시지 추가
→ 화면에 타자 효과로 출력
```

대화 흐름을 이어가기 위해 현재 입력만 보내지 않고, 최근 대화 기록을 포함한 프롬프트를 생성합니다.

```text
You are a helpful local AI chatbot in a C++ console learning project.
Use the recent conversation memory below to keep the conversation connected.
Answer the latest user message naturally.

Recent conversation memory:
User: ...
Bot: ...
User: ...

Bot:
```

## 명령어 처리 방식

현재 명령어는 크게 세 종류로 나뉩니다.

### 1. 로컬 규칙 기반 명령어

Ollama로 보내지 않고 C++ 코드 안에서 바로 처리합니다.

예시:

```text
help
hello
hi
안녕
name
study
cpp
my name is Jaemin
what is my name?
```

### 2. 프로그램 제어 명령어

대화 기록, 파일, 프로필, 서버 상태 같은 기능을 제어합니다.

예시:

```text
history
clearhistory
save
load
search keyword
profile
clearprofile
aistatus
aimodel
setaimodel modelName
exit
quit
```

### 3. AI 대화 입력

일반 문장을 입력하면 Ollama 서버가 켜져 있을 때 자동으로 AI에게 전달됩니다.

```text
You: 오늘 C++ 공부 뭐부터 할까?
```

명시적으로 AI에게 보내고 싶을 때는 `ai` 명령도 사용할 수 있습니다.

```text
You: ai C++ 포인터를 쉽게 설명해줘
```

## 출력 방식

현재 출력 방식은 두 가지입니다.

### 명령어 응답

`help`, `aistatus`, `save`, `load`, `profile` 같은 명령어 응답은 즉시 출력합니다.

이유:

- 명령어 결과는 정보 확인 목적이 강함
- 한 글자씩 출력하면 읽기 답답함
- 특히 `help`는 여러 줄이라 즉시 출력이 더 적합함

### AI 답변

Ollama에서 받은 AI 답변은 한 글자씩 출력합니다.

이유:

- ChatGPT처럼 답변이 생성되는 느낌을 주기 위함
- 긴 답변을 자연스럽게 읽을 수 있음

Ollama 응답을 기다리는 동안에는 `생각 중...` 애니메이션을 출력합니다.

## 대화 기록 삭제

`clearhistory` 명령은 세 가지 작업을 합니다.

```text
1. 메모리의 대화 기록 vector를 비움
2. 저장된 chat_history.txt 파일 삭제
3. 콘솔 화면을 비움
```

관련 함수:

```cpp
clearChatHistory(history);
deleteChatHistoryFile(chatHistoryFileName);
clearConsoleScreen();
```

따라서 `clearhistory` 실행 후에는 이전 대화가 화면에도 남지 않고, 다음 실행 때도 이전 대화 기록이 출력되지 않습니다.

## 빌드 방법

Visual Studio에서는 솔루션 파일을 열고 `Debug | x64`로 빌드하면 됩니다.

```text
Jaemin_LocalChat.sln
```

명령줄에서 MSBuild를 사용할 경우:

```powershell
cd C:\NativeClass\Jaemin_LocalChat
& 'C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64\MSBuild.exe' Jaemin_LocalChat.sln /p:Configuration=Debug /p:Platform=x64 /m
```

실행 파일 위치:

```text
C:\NativeClass\Jaemin_LocalChat\x64\Debug\Jaemin_LocalChat.exe
```

## 실행 전 확인할 것

AI 대화를 사용하려면 Ollama 서버가 실행 중이어야 합니다.

확인 방법:

```text
You: aistatus
```

서버가 꺼져 있으면 일반 입력은 로컬 규칙 기반 응답으로 처리됩니다.

Ollama 앱을 실행하거나 다음 명령으로 서버를 켤 수 있습니다.

```powershell
ollama serve
```

모델이 없다면 먼저 다운로드해야 합니다.

```powershell
ollama pull gemma4
```

## 현재 구조의 특징

- `main.cpp`는 전체 실행 흐름을 담당합니다.
- `ChatBot`은 로컬 규칙 기반 응답과 명령어 판별을 담당합니다.
- `ChatHistory`는 메모리 대화 기록과 AI 문맥 프롬프트를 담당합니다.
- `ConsoleUI`는 콘솔 출력, 타자 효과, 화면 삭제를 담당합니다.
- `FileManager`는 파일 저장/불러오기/삭제를 담당합니다.
- `UserProfile`은 사용자 이름 같은 프로필 정보를 담당합니다.
- `LocalAIConnector`는 Ollama 서버와의 연결을 담당합니다.

이 구조는 기능별 책임을 나누어 놓았기 때문에, 다음 단계에서 클래스로 전환하거나, 설정 파일을 추가하거나, GUI로 확장하기 좋습니다.

