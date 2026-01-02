# 프로젝트 개요

이미 존재하는 리듬게임을 플레이할 때 함께 실행해, 키 입력에 반응해 즉시 사운드를 내고 특정 키를 길게 누르는 동안 실시간 이펙트를 적용하는 초저지연 C++ 보조 사운드 툴을 설계합니다. 게임과는 독립적으로 동작하며, 입력→오디오 출력의 지연을 5–10ms 범위로 유지하는 것을 목표로 합니다.

---

# 핵심 목표와 요구사항

- **초저지연:** 키 입력부터 오디오 출력까지 총 지연 5–10ms. 오디오 버퍼 64–128 샘플 권장.
- **실시간 이펙트:** 롱노트 키 홀드 동안 필터/비트크러셔/리버브 등 DSP 파라미터를 매끄럽게 변조.
- **독립 실행:** 어떤 리듬게임과도 함께 실행 가능. 키 훅으로 입력만 수신, 게임과 메모리/프로세스 공유 없음.
- **안정성:** XRuns/글리치 최소화, 오디오 콜백 내 동적 할당 금지, 락프리 파이프라인.
- **유연한 매핑:** 키별로 샘플, 이펙트 프리셋, 라우팅을 사용자 설정 파일로 커스터마이즈.

---

# 아키텍처 설계

## 전체 구성도

- 입력 스레드(키 훅) → 락프리 이벤트 큐 → 오디오 엔진(콜백) → 샘플러/믹서 → FX 버스 → 오디오 출력

### 입력/키 훅

- **키 수집:** Windows는 Raw Input 또는 Low-level Keyboard Hook(WH_KEYBOARD_LL), macOS는 IOKit + NSEvent, Linux는 evdev/XInput.
- **스캔코드 기반:** 레이아웃 변화에 영향받지 않도록 스캔코드 사용.
- **리피트 무시:** 눌림/뗌만 처리. 상태 테이블로 홀드 시간 추적.
- **락프리 전달:** MPSC(Multi-Producer Single-Consumer) 락프리 큐로 오디오 스레드에 이벤트 전달.

### 오디오 엔진

- **백엔드:**
    - Windows: WASAPI Exclusive 또는 ASIO(지원 디바이스 있을 때)
    - macOS: CoreAudio
    - 크로스플랫폼: JUCE(AudioDeviceManager) 또는 PortAudio + RtAudio
- **버퍼:** 48kHz 기준 64–128 샘플(1.3–2.7ms), 안전 여유 포함 총 5–10ms 목표.
- **샘플러:** 판정음/악기 샘플은 메모리에 상주(모노/스테레오 WAV/FLAC). 시작점에 클릭 제거용 미세 페이드.
- **믹서:** 키별 채널/버스에 루팅, 마스터에서 리미터(-1dBFS)로 클리핑 보호.
- **FX 체인:** Track FX(샘플 개별) + Bus FX(전체). 파라미터는 스무딩(예: 3–10ms 리니어 램프)으로 지터 방지.

### 타이밍/스케줄링

- **고해상도 타이머:** QueryPerformanceCounter/mach_absolute_time로 타임스탬프.
- **사전 스케줄:** 오디오 콜백에서 1–2 버퍼 앞까지 이벤트를 프리페치해 경계 클릭 방지.
- **홀드 상태:** 오디오 콜백 내에서 키 상태를 원자적으로 읽고 FX 파라미터를 지속 업데이트.

---

# 모듈 설계

## 패키지 구조

- **app/**
    - **MainApp:** 초기화/런타임/GUI(간단한 HUD)
    - **ConfigManager:** JSON/INI 로드/저장
- **input/**
    - **KeyHook:** OS별 키 훅 구현
    - **KeyState:** pressed, down_ts, up_ts, hold_ms
- **audio/**
    - **AudioEngine:** 디바이스 관리, 콜백, 버퍼 파이프라인
    - **SampleManager:** 샘플 로드/프리페치/메모리 관리
    - **Mixer:** 채널/버스 믹싱, 게인/팬/리미터
    - **FX:** Filter, BitCrusher, Chorus/Flanger, Reverb, StereoWidener
    - **Smoother:** 파라미터 램핑 유틸(리니어/예상지연 보정)
- **core/**
    - **EventQueue:** 락프리 MPSC 큐
    - **NoteMap:** 키→샘플/FX 프리셋 매핑
    - **Profiler:** 지연/XRuns/CPU 사용률 측정

## 데이터 구조

- **KeyEvent:** type(down/up), scancode, timestamp_ns
- **LaneConfig:** key, sample_id, fx_preset_id, output_bus
- **FxPreset:** type, params(min/max), hold_curve, wet_range
- **Sample:** id, sample_rate, frames, channels, pointer
- **RuntimeState:** key_states[], global_wet, master_gain

---

# 기능 설계

## 버튼 매핑과 동작

- **타격 키:**
    - **다운:** 원샷 샘플 즉시 트리거(페이드-인 2–5ms)
    - **업:** 필요 시 짧은 릴리즈 테일
- **롱노트 키:**
    - **홀드 중:** 지정 FX on, 파라미터를 홀드 시간과 프리셋 곡선에 따라 변조
    - **업:** FX wet 감소 램프(50–150ms) 후 바이패스

## 이펙트 프리셋 예시

- **필터 스윕:**
    - cutoff 300Hz→8kHz, resonance 0.2→0.6, 500ms 홀드 기준 S-curve
- **비트크러셔:**
    - bitDepth 16→6, downsample 1→4, 텍스처 강화
- **리버브:**
    - mix 0→0.25, decay 0.8s, 프리딜레이 20ms, 릴리즈 시 150ms 페이드
- **스테레오 와이드너:**
    - width 0.9→1.2, 홀드 동안 점진 확장

## 설정/UX

- **HUD:** 버퍼 크기, 지연(ms), XRuns 카운트, 키 상태, FX 강도 바
- **프로필:** 게임별 키 매핑/프리셋을 프로필로 저장/로드
- **오디오 디바이스 선택:** 출력 장치/백엔드/버퍼 크기 변경, 즉시 반영

---

# 성능 및 안정화

- **콜백 내 무할당:** new/free 금지, 고정 크기 버퍼/풀 사용
- **SIMD 최적화:** NEON/AVX로 필터/크러셔 핵심 루프 가속
- **파라미터 스무딩:** 3–10ms 램프. 갑작스런 점프는 클릭 유발 방지
- **키보드 선택:** 폴링이 빠른 기계식/게이밍 키보드 권장
- **전원/우선순위:** 오디오 스레드 우선순위 상승(real-time), CPU 최소 C-state

---

# 개발 로드맵

## 마일스톤 1: 입력/출력 최소동작

- **키 훅:** 스캔코드 다운/업 수집, 리피트 무시
- **오디오 디바이스:** WASAPI/ASIO 초기화, 48kHz, 64–128 샘플 버퍼
- **원샷 샘플:** 메모리 상주, 지연 10ms 이하 확인(오실로+LED 테스트)

## 마일스톤 2: 롱노트 FX

- **필터 LPF:** 1/2/4-pole 선택, cutoff/resonance 램핑
- **홀드 매핑:** 홀드 시간→파라미터 곡선, 업 이벤트 릴리즈 램프
- **버스 구조:** Track FX + Bus FX + Master limiter

## 마일스톤 3: 프리셋/프로필/GUI

- **설정 파일:** JSON로 키→샘플/FX 프리셋
- **HUD:** 지연/XRuns/FX 강도 시각화
- **프로필:** 게임별 프리셋 스위치

## 마일스톤 4: 최적화/테스트

- **SIMD:** 핵심 DSP 루프 최적화
- **XRuns 대응:** 버퍼 크기/스레드 우선순위 튜닝
- **장치 상호운용:** 다양한 오디오 인터페이스 테스트

---

# 코드 스니펫

## 락프리 이벤트 큐(MPSC)

```cpp
struct KeyEvent {
    enum Type { Down, Up } type;
    uint32_t scancode;
    uint64_t timestampNs;
};

class EventQueue {
public:
    bool push(const KeyEvent& e) {
        auto next = (head + 1) & mask;
        if (next == tail.load(std::memory_order_acquire)) return false;
        buffer[head] = e;
        head = next;
        return true;
    }
    bool pop(KeyEvent& out) {
        auto t = tail.load(std::memory_order_acquire);
        if (t == head) return false;
        out = buffer[t];
        tail.store((t + 1) & mask, std::memory_order_release);
        return true;
    }
private:
    static constexpr size_t capacity = 1024;
    static constexpr size_t mask = capacity - 1;
    std::array<KeyEvent, capacity> buffer{};
    std::atomic<size_t> tail{0};
    size_t head{0};
};
```

## 파라미터 스무딩 유틸

```cpp
struct Smoother {
    float value = 0.0f;
    float target = 0.0f;
    float coeff = 0.0f; // 0..1 per-sample
    void setTimeMs(float ms, float sampleRate) {
        float samples = ms * sampleRate / 1000.0f;
        coeff = (samples > 1.0f) ? 1.0f / samples : 1.0f;
    }
    inline float step() {
        value += coeff * (target - value);
        return value;
    }
};
```

## 간단 LPF FX(1-pole) 예시

```cpp
class OnePoleLPF {
public:
    void setup(double sr) { sampleRate = sr; }
    void setCutoff(float hz) { cutoffTarget = hz; }
    float process(float x) {
        // cutoff 스무딩
        cutoff += 0.002f * (cutoffTarget - cutoff);
        // 1-pole 저역필터
        float a = cutoff / (cutoff + static_cast<float>(sampleRate));
        y += a * (x - y);
        return y;
    }
private:
    double sampleRate = 48000.0;
    float cutoff = 300.0f, cutoffTarget = 300.0f, y = 0.0f;
};
```

## 오디오 콜백 뼈대(JUCE 스타일)

```cpp
void getNextAudioBlock(const juce::AudioSourceChannelInfo& info) {
    auto* outL = info.buffer->getWritePointer(0, info.startSample);
    auto* outR = info.buffer->getNumChannels() > 1 ? info.buffer->getWritePointer(1, info.startSample) : nullptr;
    int n = info.numSamples;

    // 이벤트 소비 (버퍼 경계 앞당겨 처리)
    KeyEvent ev;
    while (eventQueue.pop(ev)) {
        if (ev.type == KeyEvent::Down) keyStates[ev.scancode].pressed = true;
        else keyStates[ev.scancode].pressed = false;
        keyStates[ev.scancode].lastTs = ev.timestampNs;
        // 샘플 트리거
        if (ev.type == KeyEvent::Down) sampler.trigger(scancodeToSample(ev.scancode));
    }

    // 샘플 믹싱
    for (int i = 0; i < n; ++i) {
        float mixL = sampler.nextSampleL();
        float mixR = sampler.nextSampleR();

        // 롱노트 FX 파라미터 업데이트
        bool hold = keyStates[longNoteScan].pressed;
        filter.setCutoff(hold ? 8000.0f : 800.0f);
        float fxL = filter.process(mixL);
        float fxR = filter.process(mixR);

        outL[i] = fxL;
        if (outR) outR[i] = fxR;
    }
}
```

---

# 테스트/튜닝 체크리스트

- **지연 측정:** 키보드 LED+마이크로 입력→출력 지연 기록. 목표 5–10ms.
- **버퍼 튜닝:** 64→96→128 샘플 단계별 XRuns/클릭 확인.
- **키 훅 안정성:** 게임 포커스 상관없이 이벤트 수집 확인.
- **클리핑 방지:** 마스터 리미터 테스트, -1dBFS 헤드룸 유지.
- **CPU 스파이크:** 오디오 콜백 내 할당/잠금 없음, 프로파일링으로 스파이크 제거.

---

# 다음 단계

- **플랫폼과 오디오 백엔드 선택**(예: Windows + WASAPI Exclusive vs. ASIO)을 알려주시면, 해당 환경에 맞는 스타터 프로젝트(빌드 설정, 초기화 코드, 키 훅, 샘플 로더, 필터 FX 포함)를 바로 상세화해드릴게요.
- **원하는 키 매핑/효과 스타일**(사운드볼텍스 느낌이면 필터+비트크러셔 조합이 기본)을 주시면, 프리셋과 램핑 곡선까지 구체화해 드리겠습니다.