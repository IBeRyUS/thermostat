#!/bin/bash
soc_name=$(cat /proc/cpuinfo | grep '^Hardware' | head -n1 | awk '{print toupper(substr($3,1,3))}')

if [ "BCM" == "$soc_name" ]
then
	soc_name="BROADCOM"
elif [ "SUN" == "$soc_name" ]
then
	soc_name="SUNXI"
fi

echo $soc_name
gcc -std=c11 -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -D$soc_name -o "build/main.o" "src/main.c"
gcc -o "build/thermostat" "build/main.o" -lc
