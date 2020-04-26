#!/bin/bash
gcc -std=c11 -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -o "build/main.o" "src/main.c"
gcc -o "build/thermostat" "build/main.o" -lc
