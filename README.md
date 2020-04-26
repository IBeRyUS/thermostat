# Linux SOC fan control project

I have Raspberry PI 4B, OrangePi PC Plus and OrangePi Prime SBC's. They have both debian based Linux distros. RaspberryPI has official raspbian, OrangePi SBCs have Armbian.

This project came out as necessity for setting up my first SBC OrangePi Prime as a Git server as backup for my personal projects. I assembled my SBC with active fan cooling and after a while (as expected) fan noise gave me headache :) My plan was to hook my fan to Linux thermal component to control the speed of fan. I ordered 4-pinned fan(Power, Tacho, PWM) but couldn't managed to set-up Linux thermal output for fan PWM control.

After that step back I said alright let's do easy and simple way and make ON/OFF control with simple fan. During the process I needed to monitor core temperature regularly. I decided to put temperature to the linux prompt(aka PS1). This provides you to monitor temperature everytime give a command. I tried background scripts at first but printing on stdout regularly was quite annoying. Also it has fixed frequency to print. Having temperature output in prompt gives flexibility to monitor exactly when you need.

## Temperature Monitoring on Prompt

I was gonna prepare a script to inject required commands directly to .bashrc but then I realised maybe it is not gonna suit everyone. You can find my changes to .bashrc below. 

Add the temperature() function to the .bashrc file before PS1 assignments. After this you can also use this function in all bash terminals. Starting remote scripts without terminal won't access with this setup, to make it accessible for non-terminal scripts you can add to profile file or any other method suits you.

```sh
temperature()
{
   SocTemp=`cat /etc/armbianmonitor/datasources/soctemp`
   awk '{printf ("%0.1f",$1/1000); }' <<<${SocTemp}
}
``` 

Find the PS1 assingment below on your .bashrc file. I think this is quite standard on debian based repos.(at least Raspbian and Armbian has in common)

```sh
if [ "$color_prompt" = yes ]; then
    PS1='${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w \$\[\033[00m\] '
else
    PS1='${debian_chroot:+($debian_chroot)}\u@\h:\w\$ '
fi
``` 
and change to this :
```sh
if [ "$color_prompt" = yes ]; then
    PS1='\[\033[01;32m\]'                           # change to green
    PS1="$PS1"'${debian_chroot:+($debian_chroot)}'  # chroot
    PS1="$PS1"'\u@\h '                              # user@host<space>
    PS1="$PS1"'\[\033[35m\]'                        # change to nice blue
    PS1="$PS1"'`temperature` '                      # show Temperature
    PS1="$PS1"'\[\033[01;34m\]'                     # change to brownish yellow
    PS1="$PS1"'\w'                                  # current working directory
    PS1="$PS1"'\[\033[0m\]'                         # change color
    PS1="$PS1"'\n'                                  # new line
    PS1="$PS1"'\$ '                                 # prompt: always $
else
    PS1='${debian_chroot:+($debian_chroot)}'  		# chroot
    PS1="$PS1"'\u@\h '                              # user@host<space>
    PS1="$PS1"'`temperature` '                      # show Temperature
    PS1="$PS1"'\w'                                  # current working directory
    PS1="$PS1"'\n'                                  # new line
    PS1="$PS1"'\$ '                                 # prompt: always $
fi
```

After altering .bashrc easy way to see changes is logging out and logging in again. You can also search on Google how to reload .bashrc without logging out.

Repo also have [example bashrc](/.bashrc)

## Fan Control

This is small C code and needs to be added as service. Both debian distros uses systemd daemon controller. My guide is to 
This project uses Linux sysfs(/sys) to control GPIO pins. It provides basic hysteresis ON/OFF control for the Pi devices.

SBCs usually have pins to connect peripheral devices. 

![Control Circuit Schematic](/control_schematic.PNG)