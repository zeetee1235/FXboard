# 🎹 FXBoard 사용 가이드

## ✅ 키보드로 소리 재생하기

프로그램이 실행되면 GUI 창에 **포커스**를 맞추고 다음 키를 눌러보세요:

### 키 매핑

- **A 키** → kick (킥 드럼) 🥁
- **S 키** → snare (스네어) 🥁
- **D 키** → hihat (하이햇) 🎵
- **F 키** → clap (클랩) 👏

## 🎯 중요: GUI 창에 포커스 필요!

키보드 입력을 받으려면:
1. ✅ FXBoard 창을 **클릭**하여 활성화
2. ✅ 창이 포커스되어 있는지 확인 (타이틀바 색상 변경)
3. ✅ A, S, D, F 키 누르기

## 🚀 실행 방법

```bash
# 프로그램 실행
./build/FXBoard_artefacts/FXBoard

# 또는 백그라운드로
./build/FXBoard_artefacts/FXBoard &
```

## 🎵 소리가 안 나요?

### 1. GUI 창 포커스 확인
- FXBoard 창을 클릭하세요
- 다른 창이 위에 있으면 키 입력이 안 들어갑니다

### 2. 로그 확인
터미널에 다음과 같은 메시지가 표시되어야 합니다:
```
Loaded sample: kick
Loaded sample: snare
Loaded sample: hihat
Loaded sample: clap
Key mappings configured:
  A(30) -> kick
  S(31) -> snare
  D(32) -> hihat
  F(33) -> clap
Audio initialized: 48000 Hz, 128 samples
```

### 3. 키 입력 확인
키를 누르면 로그에 표시됩니다:
```
Key pressed: a (scancode 30)
```

### 4. 샘플 파일 확인
```bash
ls -lh samples/
# kick.wav, snare.wav, hihat.wav, clap.wav가 있어야 함
```

### 5. 오디오 볼륨 확인
- 시스템 볼륨이 켜져 있는지 확인
- PulseAudio/PipeWire 믹서 확인

## 🎮 사용 예시

```
1. 프로그램 실행
   $ ./build/FXBoard_artefacts/FXBoard

2. FXBoard 창이 열림

3. 창 클릭하여 포커스

4. 키보드 연주!
   A S D F A S D F  ← 간단한 비트
   A   D   A   D    ← 킥과 하이햇
   S S S S S S S S  ← 스네어 롤
```

## 📊 GUI 정보

창에 표시되는 정보:
- **Status**: Running/Stopped
- **CPU Load**: 프로세서 사용률
- **XRuns**: 오디오 버퍼 언더런 (0이면 정상)
- **Key Hook**: Active/Inactive (현재는 GUI 키보드 사용)

## ⚙️ 커스터마이징

### 다른 샘플 사용하기

1. `samples/` 디렉토리에 WAV 파일 추가
2. 파일 이름을 `kick.wav`, `snare.wav` 등으로 지정
3. 프로그램 재실행

### 키 매핑 변경하기

`src/app/MainApp.cpp`에서 수정:
```cpp
// 현재 매핑
if (keyCode == 'A' || keyCode == 'a') { scancode = 30; matched = true; }
if (keyCode == 'S' || keyCode == 's') { scancode = 31; matched = true; }

// 예: J, K, L, ; 키로 변경
if (keyCode == 'J' || keyCode == 'j') { scancode = 30; matched = true; }
if (keyCode == 'K' || keyCode == 'k') { scancode = 31; matched = true; }
```

재빌드 후 사용:
```bash
cd build
cmake --build . -j4
```

## 🎼 더 많은 샘플 추가

현재 설정된 샘플:
- kick.wav ✅
- snare.wav ✅
- hihat.wav ✅
- clap.wav ✅
- crash.wav (추가 가능)
- tom.wav (추가 가능)

새로운 키 추가:
```cpp
// MainApp.cpp의 keyPressed 함수에 추가
else if (keyCode == 'G' || keyCode == 'g') { scancode = 34; matched = true; }
else if (keyCode == 'H' || keyCode == 'h') { scancode = 35; matched = true; }

// 그리고 매핑 추가
audioEngine->mapKeyToSample(34, "crash");
audioEngine->mapKeyToSample(35, "tom");
```

## 🐛 문제 해결

### "ALSA lib pcm.c ... Unknown PCM pulse"
- 무시해도 됩니다 (경고일 뿐)
- 오디오는 정상 작동합니다

### 키를 눌러도 아무 일도 없음
1. GUI 창 클릭 (포커스)
2. 대소문자 구분 없음 (A = a)
3. 터미널에서 "Key pressed" 로그 확인

### 소리가 이상함
- 샘플 파일이 48kHz WAV인지 확인
- 다른 샘플로 교체해보기

### CPU 사용률이 높음
- 버퍼 크기를 늘리기 (128 → 256)
- `audioEngine->initialize(256)`

## 🎉 즐기세요!

간단한 비트를 만들어보세요:

```
킥-스네어-킥-스네어:
A S A S A S A S

더블 킥:
A A S _ A A S _

하이햇 추가:
A D S D A D S D

복합 비트:
A D S D A D D F
```

---

**팁**: NumPad이나 화살표 키도 추가할 수 있습니다!
