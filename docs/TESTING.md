# FXBoard 테스트 가이드

## 🧪 빠른 테스트 (3분)

### 1단계: 권한 확인
```bash
# input 그룹에 속해있는지 확인
groups | grep input

# 없으면 추가
sudo usermod -a -G input $USER
# 로그아웃 후 재로그인 필요
```

### 2단계: 빌드
```bash
cd /home/dev/FXborad
./build.sh
```

예상 출력:
```
=== Build Complete! ===
Run: ./FXBoard_artefacts/Release/FXBoard
```

### 3단계: 실행
```bash
# 간편 실행
./start.sh

# 또는 수동 실행
sg input -c './build/FXBoard_artefacts/Release/FXBoard'
```

### 4단계: 테스트
다음 키들을 눌러보세요:
- **A, S, D, F** (왼손 홈 포지션)
- **J, K, L, ;** (오른손 홈 포지션)
- **Space, Alt** (엄지)

✅ **성공**: 키를 누를 때마다 "kick" 사운드가 들림
❌ **실패**: 아래 문제 해결 참조

## 🔍 상세 테스트

### 로그 확인
```bash
sg input -c './build/FXBoard_artefacts/Release/FXBoard 2>&1 | tee test.log'
```

**확인할 메시지:**
```
✅ Audio initialized: 48000 Hz, 128 samples
✅ Using keyboard device: /dev/input/eventXX
✅ Global keyboard hook started (evdev)
✅ Key mappings configured: A,S,D,F,J,K,L,;,Alt,Space -> kick
✅ Starting in background mode
```

### 키 입력 감지 테스트
키를 누르면 다음과 같은 로그가 나와야 합니다:
```
Key down: 30
Processing key down - scancode: 30, sampleId: kick
Triggering sample: kick, samples: 2400
Processed 1 events
```

### 성능 테스트
```bash
# CPU 사용률 확인
top -p $(pgrep FXBoard)

# 메모리 사용량 확인
ps aux | grep FXBoard | grep -v grep
```

**목표 수치:**
- CPU: < 5% (idle), < 15% (active playing)
- Memory: < 100MB
- Latency: < 10ms (체감)

## 🐛 문제 해결

### 소리가 안 나요
```bash
# 1. 샘플 파일 확인
ls samples/*.wav

# 2. 오디오 디바이스 확인
aplay -l

# 3. 볼륨 확인
amixer sget Master
```

### 키가 감지 안 됨
```bash
# 1. 권한 확인
ls -l /dev/input/event* | head -5

# 2. 키보드 디바이스 찾기
sudo evtest
# Ctrl+C로 종료하고 키보드 event 번호 확인

# 3. input 그룹 확인
groups
```

### "Permission denied" 오류
```bash
# 임시 해결 (테스트용)
sudo ./build/FXBoard_artefacts/Release/FXBoard

# 영구 해결
sudo usermod -a -G input $USER
# 로그아웃 후 재로그인
```

### 빌드 오류
```bash
# 의존성 설치
sudo dnf install -y alsa-lib-devel libX11-devel

# 클린 빌드
rm -rf build
./build.sh
```

## 📊 기능 체크리스트

- [ ] 빌드 성공
- [ ] 실행 성공 (권한 문제 없음)
- [ ] 키보드 디바이스 감지
- [ ] 10개 키 모두 소리 남 (A,S,D,F,J,K,L,;,Alt,Space)
- [ ] 백그라운드 실행 (GUI 창 없음)
- [ ] 다른 앱에서도 키 감지 (게임 등)
- [ ] 레이턴시 < 10ms (체감)
- [ ] CPU < 5% (idle)
- [ ] 안정적으로 1분 이상 실행

## 🎮 게임 통합 테스트

1. FXBoard 실행:
   ```bash
   ./start.sh
   ```

2. 브라우저나 게임 실행

3. 게임 중 키 입력:
   - A, S, D, F, J, K, L 키를 누르면서 게임 플레이
   - 소리가 게임과 함께 들려야 함
   - 게임 키 입력이 정상 작동해야 함

4. 종료:
   ```bash
   pkill FXBoard
   ```

## 📝 테스트 보고

테스트 결과를 기록하세요:

```
날짜: 2026-01-02
환경: Fedora 42, Wayland, GNOME 48
키보드: [키보드 모델]

✅ 통과
✅ 통과
❌ 실패: [문제 설명]
```

## 🚀 자동화 테스트 (개발자용)

```bash
# 전체 테스트 스위트 실행
./scripts/test.sh

# 특정 테스트만
./scripts/test.sh unit
./scripts/test.sh integration
```
