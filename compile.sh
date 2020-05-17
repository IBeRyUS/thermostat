#!/bin/bash
set -e 
soc_name=$(cat /proc/cpuinfo | grep '^Hardware' | head -n1 | awk '{print tolower(substr($3,1,3))}')
#soc_name="sun"
C_COMPILER_FLAGS="-std=c11 -O0 -g3 -Wall -Wfatal-errors -c -fmessage-length=0 -fPIC"

echo $soc_name
if [ "bcm" != "$soc_name" ] && [ "sun" != "$soc_name" ]
then
    echo "Unknown Architecture"
    exit
fi

gcc $C_COMPILER_FLAGS -o "build/bcm_port.o" "src/bcm_port.c"
gcc $C_COMPILER_FLAGS -o "build/sun_port.o" "src/sun_port.c"
gcc $C_COMPILER_FLAGS -o "build/main.o" "src/main.c"

gcc -o "build/thermostat" "build/main.o" "build/"$soc_name"_port.o" -lc
gcc -o "build/thermostat_sun" "build/main.o" "build/sun_port.o" -lc
