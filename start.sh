#!/bin/bash

echo "=== FXBoard 시작 ==="
echo "터미널에서 Ctrl+C로 종료할 수 있습니다"
echo ""

cd /home/dev/FXborad
sg input -c './build/FXBoard_artefacts/FXBoard'
