/*
 * sun_functions.c
 *
 *  Created on: 30 Apr 2020
 *      Author: IBeRyUS
 */
#include <stdio.h>
#include "soc_port.h"

const int g_argumentCount = 4;

const char* help_string = "********** Command line syntax for SunXi SoC(mostly Other Pi's) boards *********\n"
                          "* ./thermostat PORT_NO PIN_NUMBER FAN_OFF_TEMPERATURE FAN_ON_TEMPERATURE       *\n"
                          "* PORT_NO = sunxi Port name usually named A-G. Refer to the device datasheet   *\n"
                          "* PIN_NUMBER = sunxi pin number, usually put after port name A7, B5, etc.      *\n"
                          "* FAN_OFF_TEMPERATURE = Temperature value of turning off the fan.              *\n"
                          "*                       This must be lower than ON temperature                 *\n"
                          "* FAN_ON_TEMPERATURE  = Temperature value of turning on the fan.               *\n"
                          "*                       This must be higher than OFF temperature               *\n"
                          "* Example usage: \033[0;33m$ \033[0;36mthermostat A 6 35.0 36.0\033[0m                                    *\n"
                          "********************************************************************************\n";
