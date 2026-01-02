# FXBoard 사용 가이드

## ⚠️ 중요: 권한 설정 (Wayland 환경)

Fedora, Ubuntu 등 최신 리눅스에서 Wayland를 사용하는 경우, 키보드 입력에 접근하기 위해 권한 설정이 필요합니다:

```bash
# 1. 사용자를 input 그룹에 추가
sudo usermod -a -G input $USER

# 2. 로그아웃 후 다시 로그인
# (또는 현재 터미널에서만 사용하려면 sg 명령 사용)
```

### 확인 방법
```bash
# input 그룹에 속해 있는지 확인
groups | grep input
```

## 실행 방법

### 백그라운드 모드 (기본)
```bash
# 간편 실행 스크립트 사용 (권장)
./start.sh

# 또는 직접 실행
sg input -c './build/FXBoard_artefacts/FXBoard'
```
- GUI 창 없이 백그라운드에서 실행됩니다
- 게임이나 다른 애플리케이션을 사용하면서 키보드 사운드를 들을 수 있습니다
- **evdev 기반 전역 키보드 감지로 Wayland에서도 작동합니다**

### GUI 모드
```bash
sg input -c './build/FXBoard_artefacts/FXBoard --show'
```
- 설정 GUI와 함께 실행됩니다
- 키 매핑, 샘플 설정 등을 변경할 수 있습니다

## 기본 키 매핑

| 키 | 샘플 |
|----|------|
| A  | kick |
| S  | kick |
| D  | kick |
| F  | kick |
| J  | kick |
| K  | kick |
| L  | kick |
| ; (세미콜론) | kick |
| Alt | kick |
| Space | kick |

## 전역 키보드 감지

FXBoard는 Linux evdev를 사용하여 전역 키보드 입력을 감지합니다.

**특징:**
- ✅ **Wayland와 X11 모두 지원**
- ✅ 게임 중에도 키보드 사운드가 재생됩니다
- ✅ 다른 애플리케이션에 포커스가 있어도 작동합니다
- ✅ 백그라운드에서 실행되므로 화면을 차지하지 않습니다

**작동 방식:**
- `/dev/input/event*` 디바이스에서 직접 키보드 이벤트를 읽습니다
- 실제 키보드 하드웨어 레벨에서 입력을 감지합니다
- 사용자가 `input` 그룹에 속해야 접근 가능합니다

**주의사항:**
- 키보드 이벤트를 읽기만 하고 차단하지 않으므로 다른 애플리케이션도 정상 작동합니다
- 필요시 FXBoard를 종료하세요: `pkill FXBoard`

## 종료 방법

```bash
# FXBoard 프로세스 종료
pkill FXBoard

# 또는 강제 종료
pkill -9 FXBoard
```

## 트러블슈팅

### 키가 감지되지 않을 때
1. input 그룹 권한 확인:
   ```bash
   groups | grep input
   ```
   없으면:
   ```bash
   sudo usermod -a -G input $USER
   # 로그아웃 후 다시 로그인
   ```

2. 로그 확인:
   ```bash
   sg input -c './build/FXBoard_artefacts/FXBoard 2>&1 | grep -E "keyboard|hook"'
   ```
   다음 메시지가 보여야 합니다:
   - `Using keyboard device: /dev/input/eventXX`
   - `Global keyboard hook started (evdev)`

3. 키보드 디바이스 확인:
   ```bash
   ls -l /dev/input/event* | head -5
   ```
   `input` 그룹 권한이 있어야 합니다 (rw-rw----)

### 소리가 들리지 않을 때
- ALSA/PulseAudio/PipeWire가 설치되어 있는지 확인
- 볼륨이 켜져 있는지 확인
- 샘플 파일이 `samples/` 디렉토리에 있는지 확인:
  ```bash
  ls samples/*.wav
  ```

### Permission denied 오류
```bash
# 임시로 sudo 사용 (비권장)
sudo DISPLAY=:0 XAUTHORITY=$HOME/.Xauthority ./build/FXBoard_artefacts/FXBoard

# 또는 sg 명령 사용 (권장)
sg input -c './build/FXBoard_artefacts/FXBoard'
```

### Wayland vs X11
- **Wayland**: evdev 기반 키보드 감지 (권한 필요)
- **X11**: 동일하게 evdev 사용
- FXBoard는 디스플레이 서버와 무관하게 작동합니다

## 성능

- **레이턴시**: ~3-5ms (128 샘플 버퍼 @ 48kHz)
- **CPU 사용률**: 백그라운드 ~1-2%
- **메모리**: ~50MB

## 설정 파일

`config.json`에서 기본 설정을 변경할 수 있습니다:
```json
{
  "samples_dir": "samples",
  "buffer_size": 128,
  "sample_rate": 48000
}
```
