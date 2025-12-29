# FXBoard 빌드 완료! 🎉

## ✅ 빌드 성공

프로젝트가 성공적으로 컴파일되었습니다!

실행 파일 위치: `build/FXBoard_artefacts/FXBoard`

## 🎮 실행 방법

```bash
./build/FXBoard_artefacts/FXBoard
```

## ⚠️ 권한 문제 해결

### 키보드 입력 접근 권한

Linux에서 `/dev/input/` 디바이스에 접근하려면 권한이 필요합니다:

```bash
# 방법 1: 사용자를 input 그룹에 추가 (권장)
sudo usermod -a -G input $USER

# 로그아웃 후 다시 로그인하거나 재부팅
```

또는

```bash
# 방법 2: 루트 권한으로 실행 (테스트용)
sudo ./build/FXBoard_artefacts/FXBoard
```

### 키보드 디바이스 확인

사용 가능한 입력 디바이스 목록:

```bash
ls -l /dev/input/
```

키 이벤트 모니터링 (어떤 디바이스가 키보드인지 확인):

```bash
# evtest 설치
sudo dnf install evtest

# 테스트
sudo evtest
```

## 📝 현재 상태

- ✅ 오디오 엔진 초기화 성공 (48kHz, 128 샘플)
- ✅ ALSA/PipeWire 오디오 출력 연결됨
- ✅ 샘플 로더 준비됨
- ⚠️ 키보드 입력 권한 필요 (위 가이드 참조)

## 🎵 테스트 방법

1. **샘플 확인**
   ```bash
   ls -lh samples/
   ```
   - kick.wav, snare.wav, hihat.wav, clap.wav 등이 있어야 함

2. **GUI 확인**
   - 프로그램 실행 시 창이 표시됨
   - CPU 부하, XRuns 등 통계 정보 표시

3. **키 테스트** (권한 설정 후)
   - A키: kick
   - S키: snare  
   - D키: hihat
   - F키: clap

## 🐛 문제 해결

### "Unknown PCM pulse" 경고
- PipeWire/PulseAudio 호환성 경고
- 오디오는 정상 작동하므로 무시해도 됨

### "Failed to open input device"
- 위의 권한 설정 방법 참조
- 또는 config.json에서 다른 디바이스 경로 지정

### 샘플이 재생되지 않음
- samples/ 디렉토리에 WAV 파일 확인
- config.json에서 키 매핑 확인

## 📊 성능

- **버퍼 크기**: 128 샘플
- **지연**: ~2.7ms (48kHz 기준)
- **샘플레이트**: 48000 Hz

## 🎯 다음 단계

1. input 그룹에 사용자 추가 후 재로그인
2. 프로그램 재실행
3. 키보드로 샘플 트리거 테스트
4. config.json 커스터마이즈

---

더 자세한 정보는 `README.md`와 `SAMPLES_GUIDE.md`를 참고하세요!
