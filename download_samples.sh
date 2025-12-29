#!/bin/bash

# 무료 오디오 샘플 다운로드 스크립트
# 테스트용 드럼 샘플 팩을 다운로드합니다

set -e

SAMPLES_DIR="samples"
mkdir -p "$SAMPLES_DIR"

echo "=== FXBoard 샘플 다운로드 ==="
echo ""

# 옵션 1: SoX로 간단한 테스트 사운드 생성
if command -v sox &> /dev/null; then
    echo "✓ SoX가 설치되어 있습니다. 테스트 샘플을 생성합니다..."
    
    # 킥 드럼 (저음 펄스)
    sox -n "$SAMPLES_DIR/kick.wav" synth 0.3 sine 60 fade 0 0.3 0.1 gain -3
    
    # 스네어 (화이트 노이즈 + 톤)
    sox -n "$SAMPLES_DIR/snare.wav" synth 0.2 whitenoise fade 0 0.2 0.1 gain -5 : \
        synth 0.2 sine 200 fade 0 0.2 0.1 gain -5 : \
        mix
    
    # 하이햇 (고주파 노이즈)
    sox -n "$SAMPLES_DIR/hihat.wav" synth 0.1 whitenoise fade 0 0.1 0.05 highpass 8000 gain -8
    
    # 클랩 (짧은 노이즈 버스트)
    sox -n "$SAMPLES_DIR/clap.wav" synth 0.05 whitenoise fade 0 0.05 0.02 gain -3 : \
        delay 0.01 : \
        mix
    
    # 심벌 크래시
    sox -n "$SAMPLES_DIR/crash.wav" synth 1.0 whitenoise fade 0 1.0 0.5 highpass 3000 gain -10
    
    # 탐 (중음 톤)
    sox -n "$SAMPLES_DIR/tom.wav" synth 0.4 sine 120 fade 0 0.4 0.2 gain -5
    
    echo "✓ 테스트 샘플 생성 완료!"
    echo ""
    
else
    echo "⚠ SoX가 설치되어 있지 않습니다."
    echo ""
    echo "SoX 설치 방법:"
    echo "  Ubuntu/Debian: sudo apt-get install sox"
    echo "  Fedora: sudo dnf install sox"
    echo "  Arch: sudo pacman -S sox"
    echo ""
fi

# 옵션 2: 무료 샘플 팩 다운로드 (wget/curl 필요)
echo "=== 추가 샘플 다운로드 옵션 ==="
echo ""
echo "다음 사이트에서 무료 드럼 샘플을 다운로드할 수 있습니다:"
echo ""
echo "1. 99Sounds - 무료 샘플 팩"
echo "   https://99sounds.org/free-sample-packs/"
echo ""
echo "2. Freesound.org - 커뮤니티 샘플"
echo "   https://freesound.org/search/?q=drum&f=tag:kick%20tag:snare"
echo ""
echo "3. Sample Focus - 무료 샘플"
echo "   https://samplefocus.com/tags/drum"
echo ""
echo "4. Bedroom Producers Blog - 무료 팩"
echo "   https://bedroomproducersblog.com/free-samples/"
echo ""
echo "5. Drum Drops - 무료 샘플"
echo "   https://www.drumdrops.com/collections/free"
echo ""

# 샘플 정보 출력
if [ -d "$SAMPLES_DIR" ]; then
    SAMPLE_COUNT=$(find "$SAMPLES_DIR" -name "*.wav" | wc -l)
    if [ $SAMPLE_COUNT -gt 0 ]; then
        echo "=== 현재 샘플 디렉토리 ==="
        ls -lh "$SAMPLES_DIR"/*.wav 2>/dev/null || true
        echo ""
        echo "✓ $SAMPLE_COUNT 개의 샘플이 준비되었습니다!"
    else
        echo "⚠ samples/ 디렉토리에 아직 샘플이 없습니다."
    fi
fi

echo ""
echo "=== 다음 단계 ==="
echo "1. samples/ 디렉토리 확인"
echo "2. config.json에서 키 매핑 설정"
echo "3. ./build.sh로 빌드"
echo "4. 프로그램 실행 후 키 테스트"
echo ""
