# FXBoard

리듬게임용 초저지연 사운드 보조 툴

## 개요

FXBoard는 리듬게임을 플레이할 때 함께 실행하여 키 입력에 즉시 반응하는 사운드를 재생하는 C++ 기반 오디오 툴입니다. 5-10ms 이하의 초저지연을 목표로 설계되었습니다.

## 주요 기능

- **초저지연 오디오**: 키 입력부터 사운드 출력까지 5-10ms 목표
- **실시간 이펙트**: 필터, 비트크러셔, 리버브 등 DSP 효과
- **독립 실행**: 모든 리듬게임과 함께 동작
- **유연한 매핑**: JSON 설정 파일로 키 매핑 커스터마이즈
- **락프리 아키텍처**: 안정적인 오디오 처리

## 프로젝트 구조

```
FXBoard/
├── CMakeLists.txt          # 빌드 설정
├── config.json             # 설정 파일
├── idea.md                 # 프로젝트 기획서
├── src/
│   ├── main.cpp           # 엔트리 포인트
│   ├── core/              # 핵심 유틸리티
│   │   ├── EventQueue.h   # 락프리 이벤트 큐
│   │   ├── KeyEvent.h     # 키 이벤트 구조
│   │   └── Smoother.h     # 파라미터 스무딩
│   ├── input/             # 입력 처리
│   │   ├── KeyHook.h/cpp  # 키보드 훅
│   │   └── KeyState.h     # 키 상태 추적
│   ├── audio/             # 오디오 엔진
│   │   ├── AudioEngine.h/cpp    # 메인 오디오 엔진
│   │   ├── SampleManager.h/cpp  # 샘플 로드/관리
│   │   ├── Mixer.h/cpp          # 믹서
│   │   └── FX.h/cpp             # 이펙트 (필터, 비트크러셔, 리버브)
│   └── app/               # 애플리케이션
│       ├── MainApp.h/cpp        # 메인 애플리케이션
│       └── ConfigManager.h/cpp  # 설정 관리
└── samples/               # 오디오 샘플 디렉토리
```

## 빌드 방법

### 필요 조건

- CMake 3.15 이상
- C++17 지원 컴파일러
- JUCE 프레임워크 (자동으로 다운로드됨)

### Linux

```bash
# 빌드 디렉토리 생성
mkdir build
cd build

# CMake 구성
cmake ..

# 빌드
cmake --build .

# 실행
./FXBoard_artefacts/FXBoard
```

### 추가 의존성 (Linux)

```bash
# Ubuntu/Debian
sudo apt-get install libasound2-dev libjack-jackd2-dev \
    libfreetype6-dev libx11-dev libxinerama-dev libxrandr-dev \
    libxcursor-dev libxcomposite-dev mesa-common-dev

# Arch Linux
sudo pacman -S alsa-lib jack2 freetype2 libx11 libxinerama \
    libxrandr libxcursor libxcomposite mesa
```

## 사용 방법

### 1. 샘플 준비

#### 옵션 A: 자동 생성 (권장)

Python으로 테스트 샘플을 즉시 생성:

```bash
python3 generate_samples_simple.py
```

6개의 드럼 샘플(kick, snare, hihat, clap, crash, tom)이 `samples/` 디렉토리에 자동으로 생성됩니다!

#### 옵션 B: 직접 다운로드

무료 샘플 다운로드 사이트:
- **99Sounds**: https://99sounds.org/free-sample-packs/
- **Freesound**: https://freesound.org/
- **Sample Focus**: https://samplefocus.com/

자세한 가이드는 `SAMPLES_GUIDE.md`를 참고하세요.

#### 옵션 C: SoX로 생성

SoX가 설치되어 있다면:

```bash
sudo dnf install sox  # Fedora
./download_samples.sh
```

### 2. 설정 파일 편집

`config.json`에서 키 매핑을 설정합니다:

```json
{
  "keymapping": {
    "30": "kick",   // 스캔코드 30 (A키) -> kick 샘플
    "31": "snare",  // 스캔코드 31 (S키) -> snare 샘플
    "32": "hihat",  // 스캔코드 32 (D키) -> hihat 샘플
    "33": "clap"    // 스캔코드 33 (F키) -> clap 샘플
  }
}
```

### 3. 실행

프로그램을 실행하면:
- 오디오 디바이스가 자동으로 초기화됩니다
- `samples/` 디렉토리의 샘플이 로드됩니다
- 키보드 입력이 감지되기 시작합니다

매핑된 키를 누르면 해당 사운드가 즉시 재생됩니다!

## 성능 최적화 팁

### 오디오 버퍼 크기

`config.json`에서 버퍼 크기를 조정할 수 있습니다:
- **64 샘플**: 최저 지연 (~1.3ms), 높은 CPU 부하
- **128 샘플**: 균형 잡힌 설정 (~2.7ms) [권장]
- **256 샘플**: 안정성 우선 (~5.3ms)

### Linux 오디오 설정

JACK 오디오 서버 사용 시:
```bash
# JACK 시작 (저지연 설정)
jackd -dalsa -dhw:0 -r48000 -p128 -n2
```

### 실시간 우선순위

더 나은 성능을 위해 실시간 우선순위로 실행:
```bash
# rtprio 권한 설정
sudo setcap 'cap_sys_nice=eip' ./FXBoard_artefacts/FXBoard

# 실행
./FXBoard_artefacts/FXBoard
```

## 이펙트 사용

런타임에 이펙트를 활성화/제어할 수 있습니다:

### 필터
- 저역 통과 필터로 사운드를 부드럽게
- 컷오프 주파수와 공진 제어

### 비트크러셔
- 비트 뎁스 감소로 레트로 사운드
- 다운샘플링으로 로파이 효과

### 리버브
- 공간감 추가
- 믹스와 감쇠 시간 제어

## 키보드 스캔코드

Linux에서 키 스캔코드 확인:
```bash
# evtest로 스캔코드 확인
sudo evtest

# 또는 xev 사용
xev | grep keycode
```

주요 키 스캔코드 (Linux):
- A: 30, S: 31, D: 32, F: 33
- J: 36, K: 37, L: 38
- 1: 2, 2: 3, 3: 4, 4: 5

## 트러블슈팅

### 오디오가 재생되지 않음
- 오디오 디바이스가 올바르게 설정되었는지 확인
- 샘플 파일이 `samples/` 디렉토리에 있는지 확인
- 로그 출력 확인

### 높은 지연
- 버퍼 크기를 줄여보세요 (64-128 샘플)
- JACK 오디오 서버 사용 고려
- CPU 절전 모드 비활성화

### 키 입력이 감지되지 않음
- `/dev/input/event*` 디바이스 권한 확인
- `input` 그룹에 사용자 추가: `sudo usermod -a -G input $USER`
- 로그아웃 후 다시 로그인

### XRuns (오디오 끊김)
- 버퍼 크기를 늘려보세요
- 백그라운드 프로세스 최소화
- CPU 거버너를 'performance'로 설정

## 개발 로드맵

- [x] 기본 오디오 엔진
- [x] 샘플 재생
- [x] 키보드 입력 (Linux 부분 구현)
- [x] 기본 FX (필터, 비트크러셔, 리버브)
- [ ] Windows/macOS 키 훅 완성
- [ ] GUI 개선 (실시간 파라미터 조정)
- [ ] 프로필 시스템
- [ ] MIDI 입력 지원
- [ ] VST 플러그인 지원

## 라이선스

이 프로젝트는 교육 및 개인 용도로 작성되었습니다.


## 참고

- JUCE Framework: https://juce.com/
- 아이디어 문서: `idea.md`
