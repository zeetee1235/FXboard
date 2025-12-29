#!/bin/bash

# FXBoard 빌드 스크립트

set -e

echo "=== FXBoard 빌드 스크립트 ==="

# 빌드 디렉토리 생성
if [ ! -d "build" ]; then
    echo "빌드 디렉토리 생성..."
    mkdir build
fi

cd build

# CMake 구성
echo "CMake 구성 중..."
cmake ..

# 빌드
echo "빌드 중..."
cmake --build . -j$(nproc)

echo ""
echo "=== 빌드 완료! ==="
echo "실행: ./FXBoard_artefacts/Debug/FXBoard"
echo ""
