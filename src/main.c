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

#define STRING_SIZE     (50U)
#define GPIO_PORT       ('A')
#define GPIO_PIN        (6U)
#define GPIO_NUMBER     (((GPIO_PORT - 'A') * 32U) + GPIO_PIN)
#define GENERIC_ERROR   (-1)
#define FAN_OFF         (0U)
#define FAN_ON          !FAN_OFF

#if defined (BROADCOM)
#define CLI_ARGUMENT_COUNT 3
#elif defined (SUNXI)
#define CLI_ARGUMENT_COUNT 4
#endif

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

void print_syntax(void)
{

#if defined (BROADCOM)
    printf( "********* Command line syntax for Broadcom SoC(mostly Raspberry) boards ********\n"
            "* ./thermostat PIN_NUMBER FAN_OFF_TEMPERATURE FAN_ON_TEMPERATURE               *\n"
            "* PIN_NUMBER = BCM pin number. It is named GPIOx in the official schematic.    *\n"
            "* FAN_OFF_TEMPERATURE = Temperature value of turning off the fan.              *\n"
            "*                       This must be lower than ON temperature                 *\n"
            "* FAN_ON_TEMPERATURE = Temperature value of turning on the fan.                *\n"
            "*                      This must be higher than OFF temperature                *\n"
            "* Example usage: \033[0;33m$ \033[0;36mthermostat 6 35.0 36.0\033[0m                                      *\n"
            "********************************************************************************\n"
            );
#elif defined (SUNXI)
    printf( "********** Command line syntax for SunXi SoC(mostly Other Pi's) boards *********\n"
            "* ./thermostat PORT_NO PIN_NUMBER FAN_OFF_TEMPERATURE FAN_ON_TEMPERATURE       *\n"
            "* PORT_NO = sunxi Port name usually named A-G. Refer to the device datasheet   *\n"
            "* PIN_NUMBER = sunxi pin number, usually put after port name A7, B5, etc.      *\n"
            "* FAN_OFF_TEMPERATURE = Temperature value of turning off the fan.              *\n"
            "*                       This must be lower than ON temperature                 *\n"
            "* FAN_ON_TEMPERATURE = Temperature value of turning on the fan.                *\n"
            "*                      This must be higher than OFF temperature                *\n"
            "* Example usage: \033[0;33m$ \033[0;36mthermostat A 6 35.0 36.0\033[0m                                    *\n"
           "********************************************************************************\n"
            );

#endif

}

int main(int argc, char **args)
{
    char gpio_str[STRING_SIZE];
    int fd;
    int temperature;
    unsigned int fan_status = FAN_OFF;

    if (CLI_ARGUMENT_COUNT != argc)
    {
        print_syntax();
    }
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
