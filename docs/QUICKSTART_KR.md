# FXBoard - 빠른 시작 가이드

> 초저지연 키보드 사운드 프로그램 (리듬 게임용)

## ⚡ 3분 만에 시작하기

### 1️⃣ 설치 (최초 1회)

```bash
# 저장소 클론
git clone https://github.com/zeetee1235/FXboard.git
cd FXboard

# 권한 설정
sudo usermod -a -G input $USER
# ⚠️ 로그아웃 후 재로그인 필요!

# 빌드
./build.sh
```

### 2️⃣ 실행

```bash
./start.sh
```

### 3️⃣ 테스트

키보드의 **A, S, D, F, J, K, L** 키를 눌러보세요! 🎵

## 🎯 핵심 기능

- ⚡ **초저지연**: < 10ms
- 🎮 **게임 호환**: 모든 앱과 병행 실행
- 🖥️ **백그라운드**: GUI 없이 실행
- 🔧 **설정 가능**: JSON 기반 키 매핑
- 🐧 **Wayland/X11**: 모두 지원

## 🎹 기본 키 매핑

| 키 | 샘플 | 용도 |
|----|------|------|
| A, S, D, F | kick | 왼손 |
| J, K, L, ; | kick | 오른손 |
| Space, Alt | kick | 엄지 |

## 📖 자세한 문서

- [설치 가이드](docs/INSTALL.md) - 상세 설치 방법
- [설정 가이드](docs/CONFIG.md) - config.json 커스터마이징
- [테스트 가이드](TESTING.md) - 문제 해결 및 테스트
- [개발 가이드](docs/DEVELOPMENT.md) - 아키텍처 및 기여 방법

## 🚀 명령어 요약

```bash
# 실행
./start.sh

# 종료
pkill FXBoard

# GUI와 함께 실행
sg input -c './build/FXBoard_artefacts/Release/FXBoard --show'

# 커스텀 설정으로 실행
./build/FXBoard_artefacts/Release/FXBoard -c my_config.json

# 도움말
./build/FXBoard_artefacts/Release/FXBoard --help
```

## 🐛 문제 해결

### 소리가 안 나요
```bash
# 권한 확인
groups | grep input

# 샘플 파일 확인
ls samples/*.wav
```

### "Permission denied" 오류
```bash
# 임시 해결
sg input -c './start.sh'

# 영구 해결
sudo usermod -a -G input $USER
# 로그아웃 후 재로그인
```

상세한 문제 해결은 [TESTING.md](TESTING.md)를 참고하세요.

## 💡 사용 예시

### 게임하면서 사용
```bash
# 1. FXBoard 실행
./start.sh

# 2. 게임 실행 (브라우저, 앱 등)
# 키를 누르면 게임과 함께 소리가 납니다!

# 3. 종료
pkill FXBoard
```

### 커스텀 사운드 사용
```bash
# 1. 샘플 파일 추가
cp my_sound.wav samples/

# 2. config.json 수정
vim config.json

# 3. 실행
./start.sh
```

## 📊 성능

- **레이턴시**: < 10ms
- **CPU**: < 5% (대기), < 15% (활성)
- **메모리**: < 100MB
- **샘플링 레이트**: 48kHz
- **버퍼 크기**: 128 samples

## 🔗 링크

- [GitHub 저장소](https://github.com/zeetee1235/FXboard)
- [이슈 트래커](https://github.com/zeetee1235/FXboard/issues)
- [변경 로그](CHANGELOG.md)

## 📄 라이선스

[라이선스 정보 추가 예정]
