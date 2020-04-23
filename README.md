# Linux SYSFS based SOC fan control project

This project uses Linux sysfs(/sys) to control GPIO pins. It provides basic hysteresis ON/OFF control for the Pi devices.

I have Raspberry PI 4B, OrangePi PC Plus and OrangePi Prime SBC's. They have both debian based Linux distros. RaspberryPI has official raspbian, OrangePi SBCs have Armbian. 

## How to control fan

SBCs usually have pins to connect peripheral devices. 

![Control Circuit Schematic](/control_schematic.PNG)