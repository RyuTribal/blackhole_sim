#!/bin/bash
set -e

mkdir -p build_web
cd build_web

/usr/lib/emscripten/emcmake cmake .. -DIS_WEB=ON

make
