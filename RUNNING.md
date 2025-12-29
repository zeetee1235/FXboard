# ✅ FXBoard 성공적으로 실행 중!

## 🎊 현재 상태

### ✅ 작동 중
- **오디오 엔진**: 48000 Hz, 128 샘플 버퍼
- **오디오 출력**: PipeWire Media Server 연결됨
- **샘플 관리자**: 준비 완료
- **FX 엔진**: 필터, 비트크러셔, 리버브 준비됨
- **GUI**: 실행 중 (CPU 부하, 상태 표시)

### ⚠️ 설정 필요
- **키보드 입력**: 권한 설정 필요 (아래 참조)

## 🔧 키보드 입력 활성화

### 방법 1: 사용자 권한 설정 (권장)

```bash
# input 그룹에 사용자 추가
sudo usermod -a -G input $USER

# 로그아웃 후 다시 로그인하거나 재부팅
# 그 후 일반 권한으로 실행 가능
./build/FXBoard_artefacts/FXBoard
```

### 방법 2: sudo로 실행 (테스트용)

```bash
sudo ./build/FXBoard_artefacts/FXBoard
```

## 🎮 사용 방법

### 키 매핑 (config.json 기준)
- **A 키** (스캔코드 30): kick.wav
- **S 키** (스캔코드 31): snare.wav
- **D 키** (스캔코드 32): hihat.wav
- **F 키** (스캔코드 33): clap.wav

### 샘플 확인
```bash
ls -lh samples/
# kick.wav, snare.wav, hihat.wav, clap.wav 등
```

### FX 테스트
프로그램이 실행 중일 때 코드에서 FX를 활성화:
- 필터: `audioEngine.enableFilter(true)`
- 비트크러셔: `audioEngine.enableBitCrusher(true)`
- 리버브: `audioEngine.enableReverb(true)`

## 📊 성능 정보

```
샘플레이트: 48000 Hz
버퍼 크기: 128 샘플
이론적 지연: ~2.7ms
실제 지연: 약 5-10ms (전체 시스템 포함)
```

## ⚡ 최적화 팁

### 지연 줄이기
```bash
# 버퍼 크기 64로 줄이기 (더 낮은 지연, 높은 CPU 사용)
./build/FXBoard_artefacts/FXBoard --buffer-size 64
```

### CPU 절전 비활성화
```bash
# CPU 거버너를 performance로 설정
sudo cpupower frequency-set -g performance
```

### JACK 오디오 사용 (선택사항)
더 낮은 지연을 위해 JACK 설치:
```bash
sudo dnf install jack-audio-connection-kit
jackd -dalsa -dhw:0 -r48000 -p64 -n2
```

## 🐛 문제 해결

### "ALSA lib pcm.c ... Unknown PCM pulse"
- **문제**: PulseAudio/PipeWire 경고
- **해결**: 무시해도 됨, 오디오는 정상 작동

### "Failed to open input device"
- **문제**: 키보드 디바이스 권한 없음
- **해결**: 위의 "키보드 입력 활성화" 참조

### 샘플이 재생되지 않음
- samples/ 디렉토리 확인
- config.json 키 매핑 확인
- 키보드 권한 확인

### GUI가 표시되지 않음
- X11/Wayland 디스플레이 확인
- `echo $DISPLAY` 확인

## 📁 프로젝트 구조

```
FXBoard/
├── build/FXBoard_artefacts/FXBoard  ← 실행 파일
├── samples/                          ← 오디오 샘플
│   ├── kick.wav
│   ├── snare.wav
│   ├── hihat.wav
│   └── clap.wav
├── config.json                       ← 설정 파일
└── src/                              ← 소스 코드
```

## 🎯 다음 단계

1. ✅ input 그룹에 사용자 추가
2. ✅ 로그아웃/로그인
3. ✅ 프로그램 실행
4. ✅ A/S/D/F 키로 테스트
5. 🎵 추가 샘플 다운로드
6. ⚙️ config.json 커스터마이즈

## 📖 추가 문서

- `README.md` - 전체 프로젝트 가이드
- `SAMPLES_GUIDE.md` - 샘플 다운로드 가이드
- `idea.md` - 원본 기획 문서

---

**축하합니다!** FXBoard가 성공적으로 실행되고 있습니다! 🚀
