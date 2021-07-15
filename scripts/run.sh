#!/bin/sh

/usr/bin/cmake --build build --config Debug --target all -j 18 -- && ./build/6502/6502Lib/M6502Lib