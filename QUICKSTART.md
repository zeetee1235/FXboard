# FXBoard 빠른 시작 가이드

## 🚀 빠른 시작 (3단계)

### 1단계: 권한 설정
```bash
sudo usermod -a -G input $USER
```
**중요**: 명령 실행 후 로그아웃했다가 다시 로그인하세요!

### 2단계: 실행
```bash
cd /home/dev/FXborad
./start.sh
```

### 3단계: 테스트
- 키보드의 `a`, `s`, `d`, `f`, `j`, `k`, `l` 키를 눌러보세요
- 소리가 나면 성공! 🎵

## 종료 방법
```bash
pkill FXBoard
```

## ❓ 문제 해결

### "Permission denied" 오류
로그아웃했다가 다시 로그인하세요. 또는:
```bash
# 현재 세션에서만 임시로 사용
sg input -c './build/FXBoard_artefacts/FXBoard'
```

### 소리가 안 나요
1. 로그 확인:
   ```bash
   ./start.sh 2>&1 | grep -E "keyboard|hook|sample"
   ```
   
2. 다음 메시지가 보여야 합니다:
   - ✅ `Using keyboard device: /dev/input/eventXX`
   - ✅ `Global keyboard hook started (evdev)`
   - ✅ `Key mappings configured: A,S,D,F,J,K,L,;,Alt,Space -> kick`

3. 키를 누르면 다음과 같은 로그가 나와야 합니다:
   - `Key down: 30` (A 키)
   - `Processing key down - scancode: 30, sampleId: kick`
   - `Triggering sample: kick, samples: 2400`

### 게임 중에 작동하나요?
✅ **네!** FXBoard는 백그라운드에서 실행되며 모든 애플리케이션에서 키를 감지합니다.

## 📚 자세한 사용법
[USAGE.md](./USAGE.md) 파일을 참고하세요.

## 🎮 게임하면서 사용하기
```bash
# 터미널에서 실행
./start.sh

# 다른 창으로 이동해서 게임 실행
# 키를 누르면 소리가 납니다!

# 종료하려면
pkill FXBoard
```

## 🛠️ 현재 설정
- **키**: A, S, D, F, J, K, L, ; (세미콜론), Alt, Space
- **샘플**: 모든 키에 kick 사운드
- **레이턴시**: ~3-5ms
- **CPU 사용률**: ~1-2%
