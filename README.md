# Jaemin_LocalChat

`Jaemin_LocalChat`은 C++ 학습을 위해 만든 로컬 콘솔 채팅 프로그램입니다.

처음에는 단순한 규칙 기반 챗봇으로 시작하지만, 현재는 로컬 Ollama 서버와 연결해 실제 AI 모델과 대화할 수 있도록 확장되어 있습니다.

## 주요 기능

- C++17 기반 콘솔 프로그램
- Visual Studio 솔루션 제공
- 역할별 `.h` / `.cpp` 파일 분리
- 규칙 기반 기본 응답
- Ollama 로컬 AI 모델 연동
- 기본 모델: `gemma4`
- `ai` 접두어 없이 일반 문장으로 AI와 대화
- Ollama 서버 상태 확인
- 대화 기록 메모리 저장
- 대화 기록 파일 저장/불러오기/삭제
- 사용자 프로필 저장
- AI 응답 대기 중 `생각 중...` 애니메이션 표시
- AI 답변 타자 효과 출력
- 명령어 응답 즉시 출력

## 프로젝트 구조

```text
Jaemin_LocalChat/
├─ README.md
├─ PROJECT_STRUCTURE.md
├─ Jaemin_LocalChat.sln
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
│  └─ UserProfile.cpp
└─ x64/
   └─ Debug/
      └─ Jaemin_LocalChat.exe
```

더 자세한 구조 설명은 [PROJECT_STRUCTURE.md](./PROJECT_STRUCTURE.md)를 참고하세요.

## 필요 환경

- Windows
- Visual Studio 2022
- C++17
- `curl`
- Ollama
- Ollama 모델 `gemma4`

## Ollama 모델은 GitHub에 포함하지 않습니다

이 프로젝트는 Ollama 모델 파일을 저장소에 포함하지 않습니다.

이유:

- 모델 파일은 보통 수 GB 이상입니다.
- GitHub 저장소에 올리기에 적합하지 않습니다.
- 모델마다 별도 라이선스 조건이 있을 수 있습니다.
- Ollama는 사용자가 직접 로컬에 모델을 다운로드해서 사용하는 방식이 일반적입니다.

따라서 이 저장소에는 소스 코드와 실행 방법만 포함합니다.

## Ollama 설치 및 모델 다운로드

먼저 Ollama를 설치합니다.

Ollama 공식 사이트:

```text
https://ollama.com
```

설치 후 `gemma4` 모델을 다운로드합니다.

```powershell
ollama pull gemma4
```

만약 PowerShell에서 `ollama` 명령이 인식되지 않는다면, 설치 경로를 직접 지정해서 실행할 수 있습니다.

```powershell
C:\Users\User\AppData\Local\Programs\Ollama\ollama.exe pull gemma4
```

## Ollama 서버 실행

Ollama 앱을 실행하거나 다음 명령으로 서버를 실행합니다.

```powershell
ollama serve
```

프로그램 안에서는 다음 명령으로 Ollama 서버 상태를 확인할 수 있습니다.

```text
aistatus
```

Ollama 서버가 실행 중이면 일반 문장을 입력했을 때 자동으로 AI 응답을 받습니다.

Ollama 서버가 꺼져 있으면 일부 입력은 로컬 규칙 기반 응답으로 처리됩니다.

## 빌드 방법

### Visual Studio에서 빌드

1. `Jaemin_LocalChat.sln` 파일을 Visual Studio로 엽니다.
2. 상단 빌드 구성을 `Debug | x64`로 선택합니다.
3. `빌드 > 솔루션 빌드`를 실행합니다.
4. 실행 파일은 아래 경로에 생성됩니다.

```text
Jaemin_LocalChat/x64/Debug/Jaemin_LocalChat.exe
```

### MSBuild로 빌드

```powershell
cd C:\NativeClass\Jaemin_LocalChat
& 'C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64\MSBuild.exe' Jaemin_LocalChat.sln /p:Configuration=Debug /p:Platform=x64 /m
```

## 실행 방법

빌드 후 실행 파일을 실행합니다.

```powershell
.\x64\Debug\Jaemin_LocalChat.exe
```

프로그램이 시작되면 다음처럼 입력할 수 있습니다.

```text
You: help
You: aistatus
You: C++ 포인터를 쉽게 설명해줘
You: clearhistory
You: exit
```

## 주요 명령어

```text
help : 명령어 도움말을 출력합니다.
aistatus : Ollama 서버가 실행 중인지 확인합니다.
aimodel : 현재 사용 중인 Ollama 모델 이름을 출력합니다.
setaimodel modelName : 사용할 Ollama 모델을 변경합니다.
history : 현재 메모리에 저장된 대화 기록을 출력합니다.
clearhistory : 메모리 대화 기록과 저장 파일을 삭제하고 콘솔 화면을 비웁니다.
save : 현재 대화 기록을 chat_history.txt에 저장합니다.
load : chat_history.txt에 저장된 대화 기록을 출력합니다.
search keyword : 현재 대화 기록에서 keyword를 검색합니다.
profile : 저장된 사용자 프로필 정보를 출력합니다.
clearprofile : 저장된 사용자 프로필 정보를 삭제합니다.
exit / quit : 데이터를 저장하고 프로그램을 종료합니다.
```

## AI 대화 방식

일반 문장을 입력하면 Ollama 서버가 실행 중일 때 자동으로 AI에게 전달됩니다.

```text
You: 오늘 C++ 공부는 뭘 하면 좋을까?
```

명시적으로 AI에게 보내고 싶다면 `ai` 명령을 사용할 수도 있습니다.

```text
You: ai C++ 참조와 포인터 차이를 설명해줘
```

AI 응답은 최근 대화 기록을 함께 참고합니다. 그래서 이전 대화 흐름을 어느 정도 이어서 답할 수 있습니다.

## 저장되는 파일

프로그램 실행 중 다음 파일이 생성될 수 있습니다.

```text
chat_history.txt
profile.txt
```

- `chat_history.txt`: 저장된 대화 기록
- `profile.txt`: 저장된 사용자 프로필

개인 대화 내용이 들어갈 수 있으므로 GitHub에는 올리지 않는 것을 권장합니다.

## GitHub에 올릴 때 제외하면 좋은 파일

다음 파일과 폴더는 빌드 결과물 또는 개인 실행 데이터이므로 저장소에서 제외하는 것이 좋습니다.

```text
.vs/
x64/
Debug/
Release/
*.obj
*.pdb
*.ilk
*.log
*.tlog
chat_history.txt
profile.txt
```

## 학습 포인트

이 프로젝트를 통해 다음 내용을 단계적으로 학습할 수 있습니다.

- C++ 함수 분리
- 헤더 파일과 cpp 파일 분리
- `struct`
- `std::vector`
- 문자열 처리
- 파일 입출력
- 콘솔 출력 제어
- Windows API 기초 사용
- 외부 명령 실행 결과 캡처
- 로컬 AI 서버와 HTTP 통신
- 간단한 대화 메모리 구성

## 참고

이 프로젝트는 학습용 프로젝트입니다.

상용 수준의 챗봇 구조, 보안 처리, 고급 JSON 파싱, 스트리밍 응답 처리, 예외 처리 등은 아직 단순화되어 있습니다.

다음 단계에서는 다음과 같은 확장을 고려할 수 있습니다.

- `.gitignore` 추가
- 설정 파일로 기본 모델명 관리
- Ollama 스트리밍 응답 처리
- JSON 파서 도입
- 클래스 기반 구조로 리팩터링
- GUI 버전 제작

