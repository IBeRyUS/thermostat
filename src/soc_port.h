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
 * @file    soc_functions.h
 * @author  IBeRyUS
 * @brief   SOC based port functions and variables API file
 */

#ifndef SRC_SOC_FUNCTIONS_H_
#define SRC_SOC_FUNCTIONS_H_

/*--------------------------------------------------------------------------------------------------
 *  PUBLIC DEFINES
 *------------------------------------------------------------------------------------------------*/

typedef struct {
    unsigned int gpioSysFsNumber;
    int hystLowTemp;
    int hystHighTemp;
} temperatureControl_t;

/*--------------------------------------------------------------------------------------------------
 *  EXPORTED VARIABLES
 *------------------------------------------------------------------------------------------------*/

bool    socPort_parseArguments(int argc, char **args, temperatureControl_t* out_params);
char*   socPort_getHelpString(void);

#endif /* SRC_SOC_FUNCTIONS_H_ */
/*--------------------------------------------------------------------------------------------------
 *  END OF FILE
 *------------------------------------------------------------------------------------------------*/
