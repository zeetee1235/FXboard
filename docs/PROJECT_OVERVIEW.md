# FXBoard 프로젝트 개요

## 📌 프로젝트 정보

**이름**: FXBoard  
**버전**: 0.2.0  
**목적**: 리듬 게임용 초저지연 키보드 사운드 프로그램  
**언어**: C++17  
**프레임워크**: JUCE 7.0.9  
**플랫폼**: Linux (Wayland/X11)  

## 🎯 프로젝트 목표

1. **초저지연 오디오**: 키 입력부터 사운드 출력까지 < 10ms
2. **안정성**: 게임 플레이 중 크래시 없음, Lock-free 아키텍처
3. **범용성**: 모든 리듬 게임/앱과 병행 실행
4. **사용 편의성**: 간단한 설치 및 설정
5. **최소 리소스**: CPU < 5%, Memory < 100MB

## 🏗️ 아키텍처

### 핵심 컴포넌트

```
┌─────────────────────────────────────────┐
│           Application Layer             │
│  (CLI, Config, Signal Handlers)         │
└────────────┬────────────────────────────┘
             │
┌────────────┴────────────────────────────┐
│          Input Layer                    │
│  KeyHook (evdev) → EventQueue           │
└────────────┬────────────────────────────┘
             │
┌────────────┴────────────────────────────┐
│          Audio Layer                    │
│  AudioEngine → Mixer → Effects → Output │
└─────────────────────────────────────────┘
```

### 데이터 흐름

```
키보드 입력 (evdev)
    ↓
EventQueue (Lock-free)
    ↓
AudioEngine (실시간 스레드)
    ↓
SampleManager (샘플 재생)
    ↓
Mixer (믹싱)
    ↓
Effects (FX - 옵션)
    ↓
Audio Output (ALSA/PulseAudio/PipeWire)
```

## 📂 디렉토리 구조

```
FXboard/
├── src/                    # 소스 코드
│   ├── main.cpp           # 진입점
│   ├── core/              # 핵심 로직
│   │   ├── Application    # 메인 앱 로직
│   │   ├── EventQueue     # Lock-free 큐
│   │   ├── KeyEvent       # 이벤트 구조
│   │   └── Smoother       # 파라미터 스무딩
│   ├── input/             # 입력 처리
│   │   ├── KeyHook        # evdev 키보드 후킹
│   │   └── KeyState       # 키 상태 추적
│   ├── audio/             # 오디오 엔진
│   │   ├── AudioEngine    # 메인 오디오 로직
│   │   ├── SampleManager  # 샘플 관리
│   │   ├── Mixer          # 오디오 믹서
│   │   └── FX             # 이펙트
│   └── app/               # 앱 레이어
│       └── ConfigManager  # 설정 관리
├── config/                # 설정 파일
│   ├── fxboard.json.example
│   └── 99-fxboard.rules   # udev 규칙
├── samples/               # 오디오 샘플
├── scripts/               # 유틸리티 스크립트
│   ├── build.sh
│   ├── install.sh
│   └── setup_permissions.sh
├── docs/                  # 문서
│   ├── INSTALL.md
│   ├── CONFIG.md
│   ├── DEVELOPMENT.md
│   └── UPGRADING.md
└── build/                 # 빌드 출력
```

## 🔧 기술 스택

### 코어
- **C++17**: 최신 C++ 표준
- **JUCE 7.0.9**: 크로스 플랫폼 오디오 프레임워크
- **CMake 3.15+**: 빌드 시스템

### 플랫폼 API
- **Linux evdev**: 저수준 키보드 입력
- **ALSA**: Linux 오디오
- **X11**: 디스플레이 서버 (레거시 지원)

### 개발 도구
- **GCC/Clang**: 컴파일러
- **Git**: 버전 관리
- **Python 3**: 샘플 생성 스크립트

## 📊 성능 특성

### 레이턴시 분석
```
키보드 입력 (evdev)         ~1ms
EventQueue 처리             ~0.1ms
오디오 버퍼 (128@48kHz)      ~2.7ms
오디오 처리                 ~1ms
출력 레이턴시               ~3ms
──────────────────────────────
총 레이턴시                 ~8ms ✅
```

### 리소스 사용
- **대기 상태**: CPU 1-2%, Memory 40-60MB
- **활성 상태**: CPU 5-15%, Memory 60-100MB
- **샘플 로드**: ~2MB per sample

## 🚦 개발 상태

### v0.2.0 (현재)
- ✅ CLI 모드
- ✅ evdev 기반 키보드 후킹
- ✅ Lock-free 아키텍처
- ✅ JSON 설정 파일
- ✅ Wayland/X11 지원
- ✅ 백그라운드 실행
- ✅ 10개 키 기본 매핑

### 계획 중
- [ ] GUI 설정 도구 (ModernUI 완성)
- [ ] Windows/macOS 지원
- [ ] 실시간 샘플 변경
- [ ] MIDI 입력 지원
- [ ] VST 플러그인 지원
- [ ] 프리셋 시스템

## 🤝 기여 가이드

### 코드 스타일
- Google C++ Style Guide 준수
- 4 스페이스 인덴트
- 명확한 변수명 사용
- 주석으로 복잡한 로직 설명

### 커밋 메시지
```
타입(범위): 간단한 설명

상세 설명 (옵션)

관련 이슈: #123
```

**타입**: feat, fix, docs, style, refactor, test, chore

### PR 프로세스
1. Fork 후 feature 브랜치 생성
2. 코드 작성 및 테스트
3. PR 생성 (설명 포함)
4. 리뷰 및 수정
5. Merge

## 📝 문서 구조

- **README.md**: 전체 개요 및 빠른 시작
- **QUICKSTART_KR.md**: 한국어 빠른 시작 가이드
- **TESTING.md**: 테스트 및 문제 해결
- **docs/INSTALL.md**: 상세 설치 가이드
- **docs/CONFIG.md**: 설정 파일 레퍼런스
- **docs/DEVELOPMENT.md**: 개발자 가이드
- **docs/UPGRADING.md**: 업그레이드 가이드
- **CHANGELOG.md**: 변경 로그

## 🔗 관련 링크

- **GitHub**: https://github.com/zeetee1235/FXboard
- **이슈**: https://github.com/zeetee1235/FXboard/issues
- **JUCE**: https://juce.com/
- **evdev**: https://www.freedesktop.org/software/libevdev/

## 📜 라이선스

[라이선스 정보 추가 예정]

## 🙏 감사

- JUCE 프레임워크
- Linux evdev 커뮤니티
- 모든 기여자들
