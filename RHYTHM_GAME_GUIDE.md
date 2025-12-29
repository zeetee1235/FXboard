# 🎮 FXBoard - Ultra-Low Latency Rhythm Game Audio Tool

리듬게임을 위한 초저지연 오디오 도구입니다. 키보드를 눌렀을 때 최소 지연시간(5-10ms)으로 소리를 재생합니다.

## ✨ 주요 기능

- **초저지연**: 5-10ms 레이턴시 (리듬게임 완벽 대응)
- **짧은 샘플**: 15-50ms 길이의 펀치감 있는 사운드
- **간편한 설정**: JSON 파일로 키 매핑 쉽게 변경
- **실시간 시각화**: 키 입력 즉시 시각 피드백
- **폴리포닉**: 동시에 여러 소리 재생 가능

## 🚀 빠른 시작

### 1. 빌드 및 실행

```bash
# 빌드
cmake -B build
cmake --build build

# 샘플 생성 (처음 한 번만)
python3 generate_rhythm_samples.py

# 샘플 복사
cp -r samples build/FXBoard_artefacts/

# 실행
./build/FXBoard_artefacts/FXBoard
```

### 2. 기본 키 매핑

창이 포커스된 상태에서 키보드를 누르세요:

```
기본 드럼 패드:
A = Kick (킥)
S = Snare (스네어)
D = Hi-Hat (하이햇)
F = Clap (클랩)

추가 사운드:
G = Rim Shot (림샷)
H = Tom (탐)
J = Crash (크래시)
K = Click (클릭)

멜로디 비프음:
Q = Beep (800Hz)
W = Beep (1000Hz)
E = Beep (1200Hz)
R = Beep (1500Hz)
```

## ⚙️ 설정하기

### 키 매핑 변경

`rhythm_config.json` 파일을 수정하세요:

```json
{
  "keyMappings": {
    "mappings": [
      {"scancode": 30, "key": "A", "sample": "kick"},
      {"scancode": 31, "key": "S", "sample": "snare"}
    ]
  }
}
```

**스캔코드 참조:**
- 숫자 행: 2-11 = 1234567890
- 상단 행: 16-25 = QWERTYUIOP
- 홈 행: 30-38 = ASDFGHJKL
- 하단 행: 44-50 = ZXCVBNM

### 레이턴시 줄이기

1. **버퍼 크기 조정** (`rhythm_config.json`):
   ```json
   {
     "audio": {
       "bufferSize": 64  // 64 또는 32로 줄이기 (기본값: 128)
     }
   }
   ```

2. **시스템 최적화**:
   ```bash
   # CPU 거버너를 performance 모드로
   sudo cpupower frequency-set -g performance
   
   # 실시간 우선순위로 실행 (선택사항)
   sudo nice -n -20 ./build/FXBoard_artefacts/FXBoard
   ```

## 🎵 커스텀 샘플 사용하기

1. **샘플 준비**:
   - WAV 형식, 48000Hz 권장
   - 15-100ms 정도의 짧은 길이
   - 모노 또는 스테레오

2. **샘플 추가**:
   ```bash
   # samples/ 디렉토리에 WAV 파일 추가
   cp mysample.wav samples/
   
   # 빌드 디렉토리로 복사
   cp -r samples build/FXBoard_artefacts/
   ```

3. **키 매핑 설정**:
   ```json
   {"scancode": 30, "key": "A", "sample": "mysample"}
   ```

## 📊 성능 모니터링

GUI 창에서 실시간 확인 가능:
- **Latency**: 현재 레이턴시 (목표: < 10ms)
- **CPU Load**: CPU 사용률
- **XRuns**: 오디오 버퍼 언더런 횟수 (0이 이상적)

### 레이턴시 색상 코드:
- 🟢 녹색: < 10ms (완벽)
- 🟡 노란색: 10-20ms (양호)
- 🔴 빨간색: > 20ms (개선 필요)

## 🎯 리듬게임 최적화 팁

### 1. 샘플 최적화
```python
# 초단타 샘플 생성
python3 generate_rhythm_samples.py

# 특징:
# - 15-50ms 길이
# - 빠른 어택
# - 명확한 피크
```

### 2. 오디오 드라이버 설정

**ALSA (추천):**
```bash
# ~/.asoundrc 파일 생성
pcm.!default {
    type hw
    card 0
    device 0
}
```

**PipeWire:**
```bash
# 버퍼 크기 설정
pw-metadata -n settings 0 clock.force-quantum 128
```

### 3. 시스템 튜닝

```bash
# 스왑 비활성화 (RAM 충분한 경우)
sudo swapoff -a

# IRQ 밸런싱 비활성화
sudo systemctl stop irqbalance

# 고해상도 타이머 활성화
echo 1 | sudo tee /proc/sys/kernel/sched_rt_runtime_us
```

## 🔧 문제 해결

### 소리가 안 나요
1. samples/ 디렉토리가 실행 파일 옆에 있는지 확인
2. 터미널 로그에서 "Loaded sample" 메시지 확인
3. 오디오 장치가 올바르게 감지되었는지 확인

### 레이턴시가 높아요
1. 버퍼 크기를 64 또는 32로 줄이기
2. CPU 거버너를 performance 모드로 변경
3. 다른 오디오 프로그램 종료
4. ALSA 직접 사용 (PipeWire 우회)

### 클릭 노이즈가 들려요
1. 샘플에 페이드 인/아웃 적용
2. 버퍼 크기 증가 (예: 256)
3. CPU 부하 줄이기

## 📝 설정 파일 예제

### 4x4 패드 레이아웃
```json
{
  "keyMappings": {
    "mappings": [
      {"scancode": 2, "key": "1", "sample": "kick1"},
      {"scancode": 3, "key": "2", "sample": "kick2"},
      {"scancode": 4, "key": "3", "sample": "snare1"},
      {"scancode": 5, "key": "4", "sample": "snare2"},
      
      {"scancode": 16, "key": "Q", "sample": "hihat1"},
      {"scancode": 17, "key": "W", "sample": "hihat2"},
      {"scancode": 18, "key": "E", "sample": "clap1"},
      {"scancode": 19, "key": "R", "sample": "clap2"},
      
      {"scancode": 30, "key": "A", "sample": "perc1"},
      {"scancode": 31, "key": "S", "sample": "perc2"},
      {"scancode": 32, "key": "D", "sample": "perc3"},
      {"scancode": 33, "key": "F", "sample": "perc4"},
      
      {"scancode": 44, "key": "Z", "sample": "fx1"},
      {"scancode": 45, "key": "X", "sample": "fx2"},
      {"scancode": 46, "key": "C", "sample": "fx3"},
      {"scancode": 47, "key": "V", "sample": "fx4"}
    ]
  }
}
```

### 멜로디 키보드
```json
{
  "keyMappings": {
    "comment": "Piano-style layout",
    "mappings": [
      {"scancode": 30, "key": "A", "sample": "note_c"},
      {"scancode": 31, "key": "S", "sample": "note_d"},
      {"scancode": 32, "key": "D", "sample": "note_e"},
      {"scancode": 33, "key": "F", "sample": "note_f"},
      {"scancode": 34, "key": "G", "sample": "note_g"},
      {"scancode": 35, "key": "H", "sample": "note_a"},
      {"scancode": 36, "key": "J", "sample": "note_b"},
      {"scancode": 37, "key": "K", "sample": "note_c2"}
    ]
  }
}
```

## 🎓 고급 기능

### FX 체인
이펙트를 활성화하려면 `rhythm_config.json` 수정:

```json
{
  "fx": {
    "filter": {
      "enabled": true,
      "cutoff": 1000.0,
      "resonance": 0.7
    },
    "bitcrusher": {
      "enabled": true,
      "bits": 8,
      "sampleRate": 8000
    }
  }
}
```

### Lock-Free 아키텍처
- MPSC (Multiple Producer Single Consumer) 큐 사용
- 오디오 스레드에서 뮤텍스 없음
- 실시간 안전 보장

## 📚 참고 자료

- [JUCE Framework](https://juce.com/)
- [ALSA 문서](https://www.alsa-project.org/)
- [Real-Time Audio Programming](https://www.rossbencina.com/code/real-time-audio-programming-101-time-waits-for-nothing)

## 🤝 기여하기

버그 리포트, 기능 제안, Pull Request 환영합니다!

## 📄 라이선스

MIT License - 자유롭게 사용하세요!

---

**Made for rhythm gamers by rhythm gamers** 🎵🎮
