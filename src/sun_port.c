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
 * @file    sun_functions.c
 * @author  IBeRyUS
 * @brief   sunxi (allwinner) SOC specific functions and variables port file
 */
#include <stdio.h>
#include <stdbool.h>
#include "soc_port.h"

/*--------------------------------------------------------------------------------------------------
 *  MODULE DEFINES
 *------------------------------------------------------------------------------------------------*/

/*
 * @brief   Argument count includes command too.
 *          Calling $ ./thermostat A 6 35.0 36.0 will have 4 arguments.
 *          When porting to new SOC don't forget to count command itself count as argument.
 *          Param1 = ./build/thermostat
 *          Param2 = A
 *          Param3 = 6
 *          Param4 = 35.0
 *          Param5 = 36.0
 */
#define ARGUMENT_COUNT      (5)

/*--------------------------------------------------------------------------------------------------
 *  MODULE VARIABLES
 *------------------------------------------------------------------------------------------------*/

static const char* g_HelpString = "********** Command line syntax for SunXi SoC(mostly Other Pi's) boards *********\n"
                          "* ./thermostat PORT_NO PIN_NUMBER FAN_OFF_TEMPERATURE FAN_ON_TEMPERATURE       *\n"
                          "* PORT_NO = sunxi Port name usually named A-G. Refer to the device datasheet   *\n"
                          "* PIN_NUMBER = sunxi pin number, usually put after port name A7, B5, etc.      *\n"
                          "* FAN_OFF_TEMPERATURE = Temperature value of turning off the fan.              *\n"
                          "*                       This must be lower than ON temperature                 *\n"
                          "* FAN_ON_TEMPERATURE  = Temperature value of turning on the fan.               *\n"
                          "*                       This must be higher than OFF temperature               *\n"
                          "* Example usage: \033[0;33m$ \033[0;36mthermostat A 6 35.0 36.0\033[0m                                    *\n"
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
