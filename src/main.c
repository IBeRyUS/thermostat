#define _POSIX_C_SOURCE 200112
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>
#include "soc_functions.h"

#define STRING_SIZE     (50U)
#define GPIO_PORT       ('A')
#define GPIO_PIN        (6U)
#define GPIO_NUMBER     (((GPIO_PORT - 'A') * 32U) + GPIO_PIN)
#define GENERIC_ERROR   (-1)
#define FAN_OFF         (0U)
#define FAN_ON          !FAN_OFF

volatile static bool is_working;

bool init_gpio(void)
{
    int init_fd;
    int result = GENERIC_ERROR;
    DIR *gpio_check;
    char gpio_str[STRING_SIZE];
    int str_length;
    struct timespec a;

    snprintf(gpio_str, STRING_SIZE, "/sys/class/gpio/gpio%u", GPIO_NUMBER);
    printf("gpio_path=%s\n", gpio_str);
    gpio_check = opendir(gpio_str);

    if (NULL == gpio_check)
    {
        //gpio not initialised
        printf("GPIO Not initalised. Started to initialise.\n");
        init_fd = open("/sys/class/gpio/export", O_WRONLY);
        printf("init_fd=%d\n", init_fd);
        if (GENERIC_ERROR != init_fd)
        {
            str_length = snprintf(gpio_str, STRING_SIZE, "%u", GPIO_NUMBER);
            result = write(init_fd, gpio_str, str_length);
            printf("init_fd write result=%d\n", result);
            close(init_fd);
            a.tv_sec = 0;
            a.tv_nsec = 40E6;
            nanosleep(&a, NULL);
            snprintf(gpio_str, STRING_SIZE, "/sys/class/gpio/gpio%u/direction", GPIO_NUMBER);
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
    closedir(gpio_check);
    if (GENERIC_ERROR == result)
    {
        printf("GPIO Init failed.\n");
    }
    return (result);
}

int read_temp(void)
{
    char gpio_str[STRING_SIZE];
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
void signal_handler(int signal)
{
    is_working = false;
}

int main(int argc, char **args)
{
    char gpio_str[STRING_SIZE];
    int fd;
    int temperature;
    unsigned int fan_status = FAN_OFF;

    if (g_argumentCount != argc)
    {
        print_syntax();
    }
    else
    {
        is_working = init_gpio();
        signal(SIGINT, signal_handler);
        signal(SIGHUP, signal_handler);

        snprintf(gpio_str, STRING_SIZE, "/sys/class/gpio/gpio%u/value", GPIO_NUMBER);
        printf("value_path=%s\n", gpio_str);
        fd = open(gpio_str, O_RDWR);
        while (is_working)
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
}
