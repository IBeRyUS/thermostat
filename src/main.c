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
#define MAIN_LOOP_SLEEP_TIME_S  (1U)
#define GENERIC_ERROR   		(-1)
#define FAN_OFF         		(0U)
#define FAN_ON          		(1U)

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
static bool check_is_gpio_exported(const unsigned int gpio_sysfs_no);
static bool init_gpio(const unsigned int gpio_sysfs_no);
static int read_temp(void);
static void fan_control(const unsigned int gpio_sysfs_no, const unsigned int new_status);
static void print_syntax(void);
static void signal_handler(int signal);
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
int main(int argc, char *args[])
{
    int ret_val = EXIT_SUCCESS;
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
        signal(SIGINT, signal_handler);
        signal(SIGHUP, signal_handler);

        while (IsWorking)
        {
            const int temperature = read_temp();
            if (temperature > temperature_control.high_temperature)
            {
                if (FAN_OFF == fan_status)
                {
                    fan_status = FAN_ON;
                    fan_control(temperature_control.gpio_sysfs_number, fan_status);
                    printf("Fan on temp = %d\n", temperature);
                }
            }
            else if (temperature < temperature_control.low_temperature)
            {
                if (FAN_OFF != fan_status)
                {
                    fan_status = FAN_OFF;
                    fan_control(temperature_control.gpio_sysfs_number, fan_status);
                    printf("Fan off temp = %d\n", temperature);
                }
            }
            sleep(MAIN_LOOP_SLEEP_TIME_S);
        }
        fan_control(temperature_control.gpio_sysfs_number, FAN_OFF);
        printf("Closing thermal control\n");
    }
    return (ret_val);
}

/*--------------------------------------------------------------------------------------------------
 * MODULE FUNCTIONS
 *------------------------------------------------------------------------------------------------*/
static bool check_is_gpio_exported(const unsigned int gpio_sysfs_no)
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
static bool init_gpio(const unsigned int gpio_sysfs_no)
{
    int init_fd;
    bool result = true;
    char gpio_str[MAX_STRING_LENGTH];
    int str_length;
    int file_results;

    printf("Start GPIO Export.\n");
    if (false == check_is_gpio_exported(gpio_sysfs_no))
    {
        // gpio not exported
        init_fd = open("/sys/class/gpio/export", O_WRONLY);
        if (GENERIC_ERROR != init_fd)
        {
            str_length = snprintf(gpio_str, MAX_STRING_LENGTH, "%u", gpio_sysfs_no);
            file_results = write(init_fd, gpio_str, str_length);
            printf("Export GPIO stage ok. Write to export result=%d\n", file_results);
            close(init_fd);
            sleep(1);
        }
        else
        {
            printf("Export GPIO stage failed\n");
            result = false;
        }
    }
    else
    {
        printf("GPIO Export ok.\n");
    }

    if (false != result)
    {
        printf("Start setting GPIO direction to OUT.\n");
        snprintf(gpio_str, MAX_STRING_LENGTH, "/sys/class/gpio/gpio%u/direction", gpio_sysfs_no);
        init_fd = open(gpio_str, O_WRONLY);
        if (GENERIC_ERROR != init_fd)
        {
            file_results = write(init_fd, "out", 3);
            if (3 == file_results)
            {
                printf("GPIO direction set OK.\n");
            }
            else
            {
                printf("GPIO direction set NOK. result=%d\n", file_results);
                result = false;
            }
            close(init_fd);
        }
        else
        {
            printf("GPIO Direction file open error. Path = %s\n", gpio_str);
            result = false;
        }
    }
    return (result);
}

static int read_temp(void)
{
    char gpio_str[MAX_STRING_LENGTH];
    char c;
    int temperature_fd;
    int pos = 0U;

    temperature_fd = open("/sys/devices/virtual/thermal/thermal_zone0/temp",
    O_RDONLY);

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

static void fan_control(const unsigned int gpio_sysfs_no, const unsigned int new_status)
{
    int fd;
    int str_length;
    char gpio_str[MAX_STRING_LENGTH];

    snprintf(gpio_str, MAX_STRING_LENGTH, "/sys/class/gpio/gpio%u/value", gpio_sysfs_no);
    fd = open(gpio_str, O_RDWR);
    if (GENERIC_ERROR != fd)
    {
        str_length = snprintf(gpio_str, MAX_STRING_LENGTH, "%d", new_status);
        write(fd, gpio_str, str_length);
        close(fd);
    }
    else
    {
        printf("GPIO write value failed. path=%s value=%d\n", gpio_str, new_status);
    }
}

static void print_syntax(void)
{
    printf("%s", socPort_getHelpString());
}

static void signal_handler(int signal)
{
    (void) (signal);
    IsWorking = false;
}

/*--------------------------------------------------------------------------------------------------
 *  END OF FILE
 *------------------------------------------------------------------------------------------------*/
