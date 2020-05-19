/*!
 *  Hysteresis ON/OFF controlled fan application.
 *  To print help please execute application without any parameter.
 *
 *
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
 * @file main.c
 * @author IBeRyUS
 * @brief main application file
 */

/*--------------------------------------------------------------------------------------------------
 *  INCLUDES
 *------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>
#include "soc_port.h"

/*--------------------------------------------------------------------------------------------------
 *  MODULE DEFINES
 *------------------------------------------------------------------------------------------------*/

#define MAX_STRING_LENGTH       (50)
#define MAIN_LOOP_SLEEP_TIME_S  (1)
#define GENERIC_ERROR   (-1)
#define FAN_OFF         (0U)
#define FAN_ON          !FAN_OFF

/*--------------------------------------------------------------------------------------------------
 *  MODUL TIP TANIMLARI (MODULE TYPEDEFS)
 *------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------
 *  MODULE VARIABLES
 *------------------------------------------------------------------------------------------------*/
static volatile bool IsWorking;
//temperatureControl_t
/*--------------------------------------------------------------------------------------------------
 *  MODULE PROTOTYPES
 *------------------------------------------------------------------------------------------------*/
bool init_gpio(const unsigned int gpio_sysfs_no);
int read_temp(void);
void print_syntax(void);
void signal_handler(int signal);
//void

/*--------------------------------------------------------------------------------------------------
 *  PUBLIC FUNCTIONS
 *------------------------------------------------------------------------------------------------*/
/*
 * @brief Main Application entry function
 * @param argc Argument count when calling executable from command prompt
 * @param args Arguments pointer
 * @retval POSIX error codes.
 *         EXIT_SUCCESS (0) application terminated with success
 *         EXIT_FAILURE (1) application terminated with some failure
 */
int main(int argc, char* args[])
{
    char gpio_str[MAX_STRING_LENGTH];
    int ret_val = EXIT_SUCCESS;
    int fd;
    int temperature;
    temperatureControl_t temperature_control;
    unsigned int fan_status = FAN_OFF;

    if (false == socPort_parseArguments(argc, args, &temperature_control))
    {
        print_syntax();
        ret_val = EXIT_FAILURE;
    }
    else
    {
        IsWorking = init_gpio(temperature_control.gpio_sysfs_number);
        return EXIT_SUCCESS;
        signal(SIGINT, signal_handler);
        signal(SIGHUP, signal_handler);

        snprintf(gpio_str, MAX_STRING_LENGTH, "/sys/class/gpio/gpio%u/value", temperature_control.gpio_sysfs_number);
        printf("value_path=%s\n", gpio_str);
        fd = open(gpio_str, O_RDWR);
        while (IsWorking)
        {
            temperature = read_temp();
            if (temperature > 38000)
            {
                if (FAN_OFF == fan_status)
                {
                    write(fd, "1", 1);
                    fan_status = FAN_ON;
                    printf("Fan on temp = %d\n", temperature);
                }
            }
            else if (temperature < 36000)
            {
                if (FAN_OFF != fan_status)
                {
                    write(fd, "0", 1U);
                    fan_status = FAN_OFF;
                    printf("Fan off temp = %d\n", temperature);
                }
            }
            sleep(1U);
        }
        printf("Closing thermal control\n");
        close(fd);
    }
    return (ret_val);
}

/*--------------------------------------------------------------------------------------------------
 * MODULE FUNCTIONS
 *------------------------------------------------------------------------------------------------*/
bool check_gpio_is_exported(const unsigned int gpio_sysfs_no)
{
    char gpio_str[MAX_STRING_LENGTH];
    DIR *gpio_check;
    bool ret_val = false;

    snprintf(gpio_str, MAX_STRING_LENGTH, "/sys/class/gpio/gpio%u", gpio_sysfs_no);
    printf("gpio_path=%s\n", gpio_str);
    gpio_check = opendir(gpio_str);
    if (NULL != gpio_check)
    {
        ret_val = true;
        closedir(gpio_check);
    }
    return (ret_val);

}
bool init_gpio(const unsigned int gpio_sysfs_no)
{
    int init_fd;
    int result = GENERIC_ERROR;
    char gpio_str[MAX_STRING_LENGTH];
    int str_length;


    if (false == check_gpio_is_exported(gpio_sysfs_no))
    {
        //gpio not initialised
        printf("GPIO Not initalised. Started to initialise.\n");
        init_fd = open("/sys/class/gpio/export", O_WRONLY);
        printf("init_fd=%d\n", init_fd);
        if (GENERIC_ERROR != init_fd)
        {
            str_length = snprintf(gpio_str, MAX_STRING_LENGTH, "%u", gpio_sysfs_no);
            result = write(init_fd, gpio_str, str_length);
            printf("init_fd write result=%d\n", result);
            close(init_fd);
            sleep(1);
            snprintf(gpio_str, MAX_STRING_LENGTH, "/sys/class/gpio/gpio%u/direction", gpio_sysfs_no);
            printf("gpio_path=%s open to write ", gpio_str);
            init_fd = open(gpio_str, O_WRONLY);
            if (GENERIC_ERROR != init_fd)
            {
                printf("OK\n");
                result = write(init_fd, "out", 3);
                close(init_fd);
            }
            else
            {
                printf("NOT OK\n");
            }

        }
    }
    else
    {
        printf("GPIO already initalised. Continue to work.\n");
        result = 0U;
    }
    if (GENERIC_ERROR == result)
    {
        printf("GPIO Init failed.\n");
    }
    return (result);
}

int read_temp(void)
{
    char gpio_str[MAX_STRING_LENGTH];
    char c;
    int temperature_fd;
    int pos = 0U;

    temperature_fd = open("/sys/devices/virtual/thermal/thermal_zone0/temp", O_RDONLY);

    while (true)
    {
        read(temperature_fd, &c, 1U);
        if (isdigit(c))
        {
            gpio_str[pos++] = c;
        }
        else
        {
            gpio_str[pos] = '\0';
            break;
        }
    }
    close(temperature_fd);
    return (atoi(gpio_str));
}

void print_syntax(void)
{
    printf("%s", socPort_getHelpString());
}


void signal_handler(int signal)
{
    (void)(signal);
    IsWorking = false;
}

/*--------------------------------------------------------------------------------------------------
 *  END OF FILE
 *------------------------------------------------------------------------------------------------*/
