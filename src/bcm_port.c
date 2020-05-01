/*
 * bcm_functions.c
 *
 *  Created on: 30 Apr 2020
 *      Author: IBeRyUS
 */
#include <stdio.h>
#include "soc_port.h"

const int g_argumentCount = 3;

const char* help_string = "********* Command line syntax for Broadcom SoC(mostly Raspberry) boards ********\n"
                          "* ./thermostat PIN_NUMBER FAN_OFF_TEMPERATURE FAN_ON_TEMPERATURE               *\n"
                          "* PIN_NUMBER = BCM pin number. It is named GPIOx in the official schematic.    *\n"
                          "* FAN_OFF_TEMPERATURE = Temperature value of turning off the fan.              *\n"
                          "*                       This must be lower than ON temperature                 *\n"
                          "* FAN_ON_TEMPERATURE  = Temperature value of turning on the fan.               *\n"
                          "*                       This must be higher than OFF temperature               *\n"
                          "* Example usage: \033[0;33m$ \033[0;36mthermostat 6 35.0 36.0\033[0m                                      *\n"
                          "********************************************************************************\n";
