/*
 *  Copyright (c) 2020 IBeRyUS - mixer_opq@hotmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file    bcm_functions.c
 * @author  IBeRyUS
 * @brief   BCM (Broadcom) SOC specific functions and variables port file
 */
#include <stdio.h>
#include <stdbool.h>
#include "soc_port.h"

/*--------------------------------------------------------------------------------------------------
 *  MODULE VARIABLES
 *------------------------------------------------------------------------------------------------*/

static const int g_ArgumentCount = 3;

static const char* g_HelpString = "********* Command line syntax for Broadcom SoC(mostly Raspberry) boards ********\n"
                           "* ./thermostat PIN_NUMBER FAN_OFF_TEMPERATURE FAN_ON_TEMPERATURE               *\n"
                           "* PIN_NUMBER = BCM pin number. It is named GPIOx in the official schematic.    *\n"
                           "* FAN_OFF_TEMPERATURE = Temperature value of turning off the fan.              *\n"
                           "*                       This must be lower than ON temperature                 *\n"
                           "* FAN_ON_TEMPERATURE  = Temperature value of turning on the fan.               *\n"
                           "*                       This must be higher than OFF temperature               *\n"
                           "* Example usage: \033[0;33m$ \033[0;36mthermostat 6 35.0 36.0\033[0m                                      *\n"
                           "********************************************************************************\n";

/*--------------------------------------------------------------------------------------------------
 *  PUBLIC FUNCTIONS
 *------------------------------------------------------------------------------------------------*/
bool socPort_parseArguments(int argc, char **args, temperatureControl_t* out_params)
{
    bool ret_val = true;
    return (ret_val);
}

char*   socPort_getHelpString(void)
{
    return ((char *)g_HelpString);
}

/*--------------------------------------------------------------------------------------------------
 *  END OF FILE
 *------------------------------------------------------------------------------------------------*/
