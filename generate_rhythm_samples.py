#!/usr/bin/env python3
"""
리듬게임용 초단타 샘플 생성기
- 매우 짧은 길이 (50-100ms)
- 명확한 어택
- 최소 레이턴시
"""

import numpy as np
import wave
import struct
import os

SAMPLE_RATE = 48000
OUTPUT_DIR = "samples"

def write_wav(filename, samples, sample_rate=SAMPLE_RATE):
    """WAV 파일 저장"""
    samples = np.clip(samples, -1.0, 1.0)
    samples_int16 = (samples * 32767).astype(np.int16)
    
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    filepath = os.path.join(OUTPUT_DIR, filename)
    
    with wave.open(filepath, 'w') as wav_file:
        wav_file.setnchannels(1)
        wav_file.setsampwidth(2)
        wav_file.setframerate(sample_rate)
        wav_file.writeframes(samples_int16.tobytes())
    
    print(f"Created: {filepath} ({len(samples)/sample_rate*1000:.1f}ms, {len(samples_int16.tobytes())/1024:.1f}KB)")

def generate_click(duration_ms=30):
    """매우 짧은 클릭 사운드 - 리듬게임 기본음"""
    duration = int(SAMPLE_RATE * duration_ms / 1000)
    t = np.linspace(0, duration_ms/1000, duration)
    
    # 높은 주파수 클릭
    click = np.sin(2 * np.pi * 2000 * t) * 0.8
    # 빠른 감쇠
    envelope = np.exp(-t * 100)
    
    return click * envelope

def generate_snappy_kick(duration_ms=50):
    """펀치감 있는 초단타 킥"""
    duration = int(SAMPLE_RATE * duration_ms / 1000)
    t = np.linspace(0, duration_ms/1000, duration)
    
    # 베이스 주파수 스윕
    freq = 150 * np.exp(-t * 20) + 40
    phase = 2 * np.pi * np.cumsum(freq) / SAMPLE_RATE
    kick = np.sin(phase) * 0.9
    
    # 매우 빠른 감쇠
    envelope = np.exp(-t * 30)
    
    return kick * envelope

def generate_sharp_snare(duration_ms=40):
    """날카로운 스네어"""
    duration = int(SAMPLE_RATE * duration_ms / 1000)
    t = np.linspace(0, duration_ms/1000, duration)
    
    # 톤 + 노이즈
    tone = np.sin(2 * np.pi * 200 * t) * 0.3
    noise = np.random.normal(0, 0.4, duration)
    
    # 빠른 감쇠
    envelope = np.exp(-t * 40)
    
    return (tone + noise) * envelope

def generate_tight_hihat(duration_ms=20):
    """타이트한 하이햇"""
    duration = int(SAMPLE_RATE * duration_ms / 1000)
    t = np.linspace(0, duration_ms/1000, duration)
    
    # 고주파 노이즈
    noise = np.random.normal(0, 0.5, duration)
    
    # 필터링 (고역 강조)
    from scipy import signal
    b, a = signal.butter(4, 8000, 'high', fs=SAMPLE_RATE)
    filtered = signal.filtfilt(b, a, noise)
    
    # 매우 빠른 감쇠
    envelope = np.exp(-t * 100)
    
    return filtered * envelope * 0.7

def generate_sharp_clap(duration_ms=35):
    """날카로운 클랩"""
    duration = int(SAMPLE_RATE * duration_ms / 1000)
    t = np.linspace(0, duration_ms/1000, duration)
    
    # 다중 노이즈 버스트
    clap = np.zeros(duration)
    for i in range(3):
        start = int(i * duration / 20)
        end = min(start + int(duration / 30), duration)
        clap[start:end] += np.random.normal(0, 0.4, end - start)
    
    # 빠른 감쇠
    envelope = np.exp(-t * 50)
    
    return clap * envelope

def generate_rim_shot(duration_ms=15):
    """림샷 - 매우 짧고 날카로움"""
    duration = int(SAMPLE_RATE * duration_ms / 1000)
    t = np.linspace(0, duration_ms/1000, duration)
    
    # 고주파 톤
    rim = np.sin(2 * np.pi * 1500 * t) * 0.7
    
    # 극도로 빠른 감쇠
    envelope = np.exp(-t * 150)
    
    return rim * envelope

def generate_beep(duration_ms=25, freq=1000):
    """비프음 - 테스트용"""
    duration = int(SAMPLE_RATE * duration_ms / 1000)
    t = np.linspace(0, duration_ms/1000, duration)
    
    beep = np.sin(2 * np.pi * freq * t) * 0.6
    envelope = np.exp(-t * 60)
    
    return beep * envelope

if __name__ == "__main__":
    print("Generating rhythm game samples...")
    print(f"Sample rate: {SAMPLE_RATE} Hz")
    print()
    
    # 리듬게임용 초단타 샘플 생성
    write_wav("click.wav", generate_click(30))
    write_wav("kick.wav", generate_snappy_kick(50))
    write_wav("snare.wav", generate_sharp_snare(40))
    
    # scipy 없으면 간단한 hihat
    try:
        write_wav("hihat.wav", generate_tight_hihat(20))
    except ImportError:
        print("Warning: scipy not found, using simple hihat")
        duration = int(SAMPLE_RATE * 0.02)
        t = np.linspace(0, 0.02, duration)
        noise = np.random.normal(0, 0.5, duration)
        envelope = np.exp(-t * 100)
        write_wav("hihat.wav", noise * envelope * 0.7)
    
    write_wav("clap.wav", generate_sharp_clap(35))
    write_wav("rim.wav", generate_rim_shot(15))
    
    # 다양한 비프음 (키보드 행별로)
    write_wav("beep1.wav", generate_beep(25, 800))
    write_wav("beep2.wav", generate_beep(25, 1000))
    write_wav("beep3.wav", generate_beep(25, 1200))
    write_wav("beep4.wav", generate_beep(25, 1500))
    
    print()
    print("✓ All rhythm game samples generated!")
    print("  - Ultra-low latency optimized")
    print("  - Short duration (15-50ms)")
    print("  - Perfect for rhythm games")
