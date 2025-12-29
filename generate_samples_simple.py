#!/usr/bin/env python3
"""
FXBoard 테스트 샘플 생성기 (numpy만 사용)
간단한 드럼 사운드를 생성합니다.
"""

import numpy as np
import struct
import os

SAMPLE_RATE = 48000

def write_wav(filename, audio_data, sample_rate=48000):
    """WAV 파일 작성 (numpy만 사용)"""
    # 16-bit PCM으로 변환
    audio_int16 = (audio_data * 32767).astype(np.int16)
    
    # WAV 헤더 작성
    with open(filename, 'wb') as f:
        # RIFF 헤더
        f.write(b'RIFF')
        f.write(struct.pack('<I', 36 + len(audio_int16) * 2))  # 파일 크기
        f.write(b'WAVE')
        
        # fmt 청크
        f.write(b'fmt ')
        f.write(struct.pack('<I', 16))  # fmt 청크 크기
        f.write(struct.pack('<H', 1))   # PCM 포맷
        f.write(struct.pack('<H', 1))   # 모노
        f.write(struct.pack('<I', sample_rate))  # 샘플레이트
        f.write(struct.pack('<I', sample_rate * 2))  # 바이트레이트
        f.write(struct.pack('<H', 2))   # 블록 정렬
        f.write(struct.pack('<H', 16))  # 비트 뎁스
        
        # data 청크
        f.write(b'data')
        f.write(struct.pack('<I', len(audio_int16) * 2))
        f.write(audio_int16.tobytes())

def generate_kick(duration=0.3):
    """킥 드럼"""
    t = np.linspace(0, duration, int(SAMPLE_RATE * duration))
    freq = np.linspace(150, 40, len(t))
    phase = 2 * np.pi * np.cumsum(freq) / SAMPLE_RATE
    signal = np.sin(phase) * np.exp(-5 * t)
    return signal / np.max(np.abs(signal)) * 0.8

def generate_snare(duration=0.2):
    """스네어"""
    t = np.linspace(0, duration, int(SAMPLE_RATE * duration))
    tone = np.sin(2 * np.pi * 200 * t) * np.exp(-10 * t)
    noise = np.random.uniform(-1, 1, len(t)) * np.exp(-8 * t)
    signal = 0.6 * tone + 0.4 * noise
    return signal / np.max(np.abs(signal)) * 0.7

def generate_hihat(duration=0.1):
    """하이햇"""
    t = np.linspace(0, duration, int(SAMPLE_RATE * duration))
    noise = np.random.uniform(-1, 1, len(t))
    # 간단한 하이패스
    noise = np.diff(noise, prepend=noise[0])
    signal = noise * np.exp(-30 * t)
    return signal / np.max(np.abs(signal)) * 0.5

def generate_clap(duration=0.1):
    """클랩"""
    t = np.linspace(0, duration, int(SAMPLE_RATE * duration))
    clap = np.zeros(len(t))
    for delay in [0, 0.01, 0.02]:
        idx = int(delay * SAMPLE_RATE)
        burst_len = int(0.03 * SAMPLE_RATE)
        if idx + burst_len < len(t):
            burst = np.random.uniform(-1, 1, burst_len)
            burst *= np.exp(-50 * np.linspace(0, 0.03, burst_len))
            clap[idx:idx+burst_len] += burst
    return clap / np.max(np.abs(clap)) * 0.8

def generate_crash(duration=1.5):
    """크래시 심벌"""
    t = np.linspace(0, duration, int(SAMPLE_RATE * duration))
    noise = np.random.uniform(-1, 1, len(t))
    for _ in range(3):
        noise = np.diff(noise, prepend=noise[0])
    signal = noise * np.exp(-2 * t)
    return signal / np.max(np.abs(signal)) * 0.6

def generate_tom(duration=0.4):
    """탐"""
    t = np.linspace(0, duration, int(SAMPLE_RATE * duration))
    freq = np.linspace(180, 120, len(t))
    phase = 2 * np.pi * np.cumsum(freq) / SAMPLE_RATE
    signal = np.sin(phase) * np.exp(-7 * t)
    return signal / np.max(np.abs(signal)) * 0.8

def main():
    os.makedirs('samples', exist_ok=True)
    
    print("=== FXBoard 테스트 샘플 생성 ===\n")
    
    samples_to_generate = [
        ('kick.wav', generate_kick),
        ('snare.wav', generate_snare),
        ('hihat.wav', generate_hihat),
        ('clap.wav', generate_clap),
        ('crash.wav', generate_crash),
        ('tom.wav', generate_tom),
    ]
    
    for filename, generator in samples_to_generate:
        filepath = os.path.join('samples', filename)
        print(f"생성 중: {filename}...", end=' ')
        
        audio = generator()
        write_wav(filepath, audio, SAMPLE_RATE)
        
        file_size = os.path.getsize(filepath) / 1024
        print(f"✓ ({file_size:.1f} KB)")
    
    print(f"\n✓ {len(samples_to_generate)}개의 샘플 생성 완료!")
    print(f"위치: {os.path.abspath('samples')}/")
    print("\n생성된 파일:")
    for f, _ in samples_to_generate:
        print(f"  - {f}")
    
    print("\n다음 단계:")
    print("1. config.json 확인 (키 매핑)")
    print("2. ./build.sh로 빌드")
    print("3. 실행 후 A(kick), S(snare), D(hihat), F(clap) 키 테스트")

if __name__ == '__main__':
    try:
        main()
    except ImportError:
        print("❌ numpy가 필요합니다.")
        print("\n설치:")
        print("  pip3 install numpy")
        print("또는:")
        print("  sudo dnf install python3-numpy")
