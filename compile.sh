#!/bin/bash
soc_name=$(cat /proc/cpuinfo | grep '^Hardware' | head -n1 | awk '{print toupper(substr($3,1,3))}')

C_COMPILER_FLAGS="-std=c11 -O0 -g3 -Wall -Wfatal-errors -c -fmessage-length=0 -fPIC"

if [ "BCM" == "$soc_name" ]
then
    soc_file_name="bcm_functions"
elif [ "SUN" == "$soc_name" ]
then
    soc_file_name="sun_functions"
fi

echo $soc_name
gcc $C_COMPILER_FLAGS -o "build/"$soc_file_name".o" "src/"$soc_file_name".c"
gcc $C_COMPILER_FLAGS -o "build/main.o" "src/main.c"

gcc -o "build/thermostat" "build/main.o" "build/"$soc_file_name".o" -lc
