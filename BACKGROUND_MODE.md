# FXBoard 백그라운드 모드 가이드

## 개요

FXBoard는 이제 백그라운드에서 동작하며, GUI를 통해 키 매핑과 사운드 설정을 손쉽게 변경할 수 있습니다.

## 주요 기능

### 1. 백그라운드 실행
- FXBoard는 창을 닫아도 백그라운드에서 계속 실행됩니다
- 키보드 입력을 전역적으로 감지하여 사운드를 재생합니다
- 시스템 리소스를 최소화하면서 항상 대기 상태를 유지합니다

### 2. GUI 기반 키 매핑 설정
- **설정 창 열기**: 메인 윈도우에서 "Settings" 메뉴 선택
- **Key Mapping 탭**:
  - 현재 설정된 모든 키 매핑 확인
  - 새로운 키 매핑 추가
  - 기존 매핑 제거 또는 변경
  - 각 키에 할당된 샘플 변경

### 3. 샘플 관리
- **Samples 탭**:
  - 로드된 모든 샘플 확인
  - 각 샘플의 게인(볼륨) 조정 (0.0 ~ 2.0)
  - 새로운 샘플 파일 추가
  - 샘플 파일 경로 변경

### 4. 오디오 설정
- **Audio 탭**:
  - 오디오 디바이스 선택
  - 샘플 레이트 설정 (44.1kHz, 48kHz, 96kHz)
  - 버퍼 크기 설정 (64, 128, 256, 512 샘플)
  - 레이턴시 확인

## 실행 방법

### 일반 모드로 시작
```bash
./build/FXBoard_artefacts/FXBoard
```
- GUI 창이 표시됩니다
- 창을 닫으면 백그라운드로 전환됩니다

### 백그라운드 모드로 시작
```bash
./build/FXBoard_artefacts/FXBoard --hidden
# 또는
./build/FXBoard_artefacts/FXBoard -h
```
- GUI 창 없이 백그라운드에서 바로 시작됩니다
- 키보드 입력은 즉시 감지됩니다

## 창 관리

### 메인 창 다시 열기
백그라운드 모드에서 메인 창을 다시 열려면:
1. 애플리케이션 메뉴에서 "Show Main Window" 선택
2. 또는 프로그램을 다시 실행 (이미 실행 중이면 창이 나타남)

### 설정 창 열기
1. 메인 창의 메뉴에서 "Settings" 선택
2. 또는 애플리케이션 메뉴에서 "Settings" 선택

### 프로그램 종료
1. 메인 창의 "Quit" 버튼
2. 애플리케이션 메뉴에서 "Quit"
3. 또는 `Ctrl+C` (터미널에서 실행 시)

## 설정 파일

모든 설정은 `config.json` 파일에 저장됩니다:

```json
{
  "audio": {
    "sampleRate": 48000,
    "bufferSize": 128,
    "outputChannels": 2,
    "deviceName": ""
  },
  "keymapping": {
    "30": "kick",
    "31": "snare",
    "32": "hihat",
    "33": "clap"
  },
  "samples": {
    "kick": {
      "file": "samples/kick.wav",
      "gain": 1.0
    },
    "snare": {
      "file": "samples/snare.wav",
      "gain": 0.9
    }
  }
}
```

## 키 매핑 예제

### 기본 드럼 세트
- **A (scancode 30)**: Kick Drum
- **S (scancode 31)**: Snare Drum
- **D (scancode 32)**: Hi-Hat
- **F (scancode 33)**: Clap

### 확장 세트
GUI를 통해 다음 키들을 추가 매핑할 수 있습니다:
- Q, W, E, R, T (scancodes 16-20)
- Z, X, C, V, B (scancodes 44-48)
- 1, 2, 3, 4, 5 (scancodes 2-6)

## 샘플 추가하기

### GUI를 통한 추가
1. Settings → Samples 탭 열기
2. "Add New Sample" 버튼 클릭
3. "Browse..." 버튼으로 WAV 파일 선택
4. 게인 슬라이더로 볼륨 조정

### 지원 포맷
- WAV (권장)
- MP3
- OGG

## 트러블슈팅

### 키 입력이 감지되지 않음
- Linux: 권한 문제일 수 있습니다. `sudo` 권한으로 실행 시도
- 다른 애플리케이션이 키 입력을 가로채고 있는지 확인

### 오디오가 들리지 않음
1. Settings → Audio 탭에서 올바른 오디오 디바이스 선택
2. 시스템 볼륨 확인
3. 샘플 파일이 올바르게 로드되었는지 확인

### 레이턴시가 높음
1. Settings → Audio 탭에서 버퍼 크기를 낮춤 (64 또는 128)
2. CPU 사용량 확인 (높으면 버퍼 크기 증가 필요)

## 성능 최적화

### CPU 사용량 감소
- 사용하지 않는 샘플 제거
- 버퍼 크기 증가 (256 또는 512)
- FX 효과 비활성화

### 레이턴시 감소
- 버퍼 크기 감소 (64 또는 128)
- JACK 오디오 서버 사용 (Linux)
- 고성능 오디오 인터페이스 사용

## 시스템 요구사항

- **OS**: Linux (X11), Windows, macOS
- **CPU**: 듀얼 코어 이상
- **RAM**: 최소 512MB
- **오디오**: ALSA, PulseAudio, JACK, WASAPI, CoreAudio

## 라이선스 및 기여

프로젝트는 현재 개발 중이며, 기여를 환영합니다!
