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
#include <stdlib.h>
#include <stdbool.h>
#include "soc_port.h"

/*--------------------------------------------------------------------------------------------------
 *  MODULE DEFINES
 *------------------------------------------------------------------------------------------------*/

/*
 * @brief   Argument count includes command too.
 *          Calling $ ./thermostat 6 35.0 36.0 will have 4 arguments.
 *          When porting to new SOC don't forget to count command itself count as argument.
 *          Param1 = ./build/thermostat
 *          Param2 = 6
 *          Param3 = 35.0
 *          Param4 = 36.0
 */
#define ARGUMENT_COUNT      (4)

#define INVALID_VALUE       (0)
#define MIN_HYSTERESIS_GAP  (1000)
/*--------------------------------------------------------------------------------------------------
 *  MODULE VARIABLES
 *------------------------------------------------------------------------------------------------*/

static const char* HelpString = "********* Command line syntax for Broadcom SoC(mostly Raspberry) boards ********\n"
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
bool socPort_parseArguments(int argc, char* args[], temperatureControl_t *const out_params)
{
    bool ret_val = false;

    if (ARGUMENT_COUNT == argc)
    {
        const unsigned int temp_sysfs_number = strtoul(args[1], NULL, 0);
        const int temp_low_temperature  = (int)(strtof(args[2], NULL) * 1000);
        const int temp_high_temperature = (int)(strtof(args[3], NULL) * 1000);

        if ((INVALID_VALUE != temp_sysfs_number) &&
            (INVALID_VALUE != temp_low_temperature) &&
            (INVALID_VALUE != temp_high_temperature) &&
            (temp_low_temperature != temp_high_temperature) &&
            (temp_low_temperature < (temp_high_temperature - MIN_HYSTERESIS_GAP))
           )
        {
            /* Arguments are OK */
            out_params->gpio_sysfs_number   = temp_sysfs_number;
            out_params->low_temperature     = temp_low_temperature;
            out_params->high_temperature    = temp_high_temperature;
            ret_val = true;
        }
    }
    return (ret_val);
}

char*   socPort_getHelpString(void)
{
    return ((char *)HelpString);
}

/*--------------------------------------------------------------------------------------------------
 *  END OF FILE
 *------------------------------------------------------------------------------------------------*/
