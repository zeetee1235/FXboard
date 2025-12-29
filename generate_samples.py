#!/usr/bin/env python3
"""
FXBoard 테스트 샘플 생성기
numpy와 scipy를 사용해 간단한 드럼 사운드를 생성합니다.
"""

import numpy as np
from scipy.io import wavfile
import os

# 샘플레이트
SAMPLE_RATE = 48000

def generate_kick(duration=0.3, freq_start=150, freq_end=40):
    """킥 드럼 사운드 생성"""
    t = np.linspace(0, duration, int(SAMPLE_RATE * duration))
    
    # 주파수 스윕 (높은 음에서 낮은 음으로)
    freq = np.linspace(freq_start, freq_end, len(t))
    phase = 2 * np.pi * np.cumsum(freq) / SAMPLE_RATE
    
    # 사인파 생성
    signal = np.sin(phase)
    
    # 엔벨로프 (빠른 어택, 중간 디케이)
    envelope = np.exp(-5 * t)
    
    # 최종 신호
    kick = signal * envelope
    
    # 정규화
    kick = kick / np.max(np.abs(kick)) * 0.8
    
    return (kick * 32767).astype(np.int16)

def generate_snare(duration=0.2):
    """스네어 드럼 사운드 생성"""
    t = np.linspace(0, duration, int(SAMPLE_RATE * duration))
    
    # 톤 성분 (200Hz 사인파)
    tone = np.sin(2 * np.pi * 200 * t)
    tone_env = np.exp(-10 * t)
    tone_signal = tone * tone_env
    
    # 노이즈 성분 (화이트 노이즈)
    noise = np.random.uniform(-1, 1, len(t))
    noise_env = np.exp(-8 * t)
    noise_signal = noise * noise_env
    
    # 믹스 (노이즈 40%, 톤 60%)
    snare = 0.4 * noise_signal + 0.6 * tone_signal
    
    # 정규화
    snare = snare / np.max(np.abs(snare)) * 0.7
    
    return (snare * 32767).astype(np.int16)

def generate_hihat(duration=0.1, is_open=False):
    """하이햇 사운드 생성"""
    if is_open:
        duration = 0.3
    
    t = np.linspace(0, duration, int(SAMPLE_RATE * duration))
    
    # 고주파 노이즈
    noise = np.random.uniform(-1, 1, len(t))
    
    # 하이패스 필터 (간단한 차분)
    noise = np.diff(noise, prepend=noise[0])
    noise = np.diff(noise, prepend=noise[0])
    
    # 엔벨로프
    if is_open:
        envelope = np.exp(-3 * t)  # 느린 디케이
    else:
        envelope = np.exp(-30 * t)  # 빠른 디케이
    
    hihat = noise * envelope
    
    # 정규화
    hihat = hihat / np.max(np.abs(hihat)) * 0.5
    
    return (hihat * 32767).astype(np.int16)

def generate_clap(duration=0.1):
    """클랩 사운드 생성"""
    t = np.linspace(0, duration, int(SAMPLE_RATE * duration))
    
    # 여러 개의 짧은 노이즈 버스트
    clap = np.zeros(len(t))
    
    # 3개의 버스트 (약간씩 지연)
    for delay in [0, 0.01, 0.02]:
        start_idx = int(delay * SAMPLE_RATE)
        burst_len = int(0.03 * SAMPLE_RATE)
        if start_idx + burst_len < len(t):
            burst = np.random.uniform(-1, 1, burst_len)
            burst_env = np.exp(-50 * np.linspace(0, 0.03, burst_len))
            clap[start_idx:start_idx+burst_len] += burst * burst_env
    
    # 정규화
    clap = clap / np.max(np.abs(clap)) * 0.8
    
    return (clap * 32767).astype(np.int16)

def generate_crash(duration=1.5):
    """크래시 심벌 사운드 생성"""
    t = np.linspace(0, duration, int(SAMPLE_RATE * duration))
    
    # 복잡한 고주파 노이즈
    crash = np.random.uniform(-1, 1, len(t))
    
    # 하이패스 필터
    for _ in range(3):
        crash = np.diff(crash, prepend=crash[0])
    
    # 느린 디케이 엔벨로프
    envelope = np.exp(-2 * t)
    
    crash = crash * envelope
    
    # 정규화
    crash = crash / np.max(np.abs(crash)) * 0.6
    
    return (crash * 32767).astype(np.int16)

def generate_tom(duration=0.4, freq=120):
    """탐 드럼 사운드 생성"""
    t = np.linspace(0, duration, int(SAMPLE_RATE * duration))
    
    # 주파수 스윕
    freq_sweep = np.linspace(freq * 1.5, freq, len(t))
    phase = 2 * np.pi * np.cumsum(freq_sweep) / SAMPLE_RATE
    
    signal = np.sin(phase)
    
    # 엔벨로프
    envelope = np.exp(-7 * t)
    
    tom = signal * envelope
    
    # 정규화
    tom = tom / np.max(np.abs(tom)) * 0.8
    
    return (tom * 32767).astype(np.int16)

def main():
    # samples 디렉토리 생성
    os.makedirs('samples', exist_ok=True)
    
    print("=== FXBoard 테스트 샘플 생성 ===\n")
    
    samples = {
        'kick.wav': lambda: generate_kick(),
        'snare.wav': lambda: generate_snare(),
        'hihat.wav': lambda: generate_hihat(is_open=False),
        'hihat_open.wav': lambda: generate_hihat(is_open=True),
        'clap.wav': lambda: generate_clap(),
        'crash.wav': lambda: generate_crash(),
        'tom.wav': lambda: generate_tom(freq=120),
    }
    
    for filename, generator in samples.items():
        filepath = os.path.join('samples', filename)
        print(f"생성 중: {filename}...", end=' ')
        
        audio = generator()
        wavfile.write(filepath, SAMPLE_RATE, audio)
        
        file_size = os.path.getsize(filepath) / 1024  # KB
        print(f"✓ ({file_size:.1f} KB)")
    
    print(f"\n✓ {len(samples)}개의 샘플이 생성되었습니다!")
    print(f"디렉토리: {os.path.abspath('samples')}")
    print("\n다음 단계:")
    print("1. config.json에서 키 매핑 확인")
    print("2. ./build.sh로 프로그램 빌드")
    print("3. 실행 후 A, S, D, F 키 테스트")

if __name__ == '__main__':
    try:
        main()
    except ImportError as e:
        print("❌ 필요한 라이브러리가 설치되어 있지 않습니다.")
        print("\n설치 방법:")
        print("  pip3 install numpy scipy")
        print("\n또는:")
        print("  sudo dnf install python3-numpy python3-scipy")
        print(f"\n오류: {e}")
