#!/usr/bin/python3
# -*- coding: utf-8 -*-
"""
This is an example of connecting LIME2 + LIME2-SHIELD + MOD-IO (over UEXT) and controlling a relay (on / off).
It's mixed with slapos / re6stnet install.


I. prepare lime2 + lime2's shield + MOD-IO

# install packages at your laptop
root@t470:~/tmp$ apt install minicom

# XXX: currently nexedi suports fro armhf only debian (slapos + re6stnet packages)
ivan@t470:~/tmp$ wget http://images.olimex.com/release/a20/A20-OLinuXino-buster-minimal-20200601-122725.img.7z
ivan@t470:~/tmp$ 7z e A20-OLinuXino-buster-minimal-20200601-122725.img.7z

# download from https://www.balena.io/etcher/
ivan@t470:~/tmp$ unzip ../balena-etcher-electron-1.5.95-linux-x64.zip
ivan@t470:~/tmp$ chmod o+x balenaEtcher-1.5.95-x64.AppImage
ivan@t470:~/tmp$ ./balenaEtcher-1.5.95-x64.AppImage
# flash to micro-sd card the previously download image

# put micro-sd card inside lime2 (not shield!)

# put lime2 shield on lime2 board

# charge lime2 board with 5V adaptor

# connect USB cable to USB1 on lime2 board and a laptop

# wait till you see a green light at the board
root@t470:~$ minicom -D /dev/ttyACM0

# username and password are olimex / olimex 

# put wifi dongle (or ethernet cable) but get an ipV4 address to ssh on

ssh olimex@<ipv4_address> (password: olimex)

# update software
root@a20-olinuxino:~# apt update
root@a20-olinuxino:~# apt upgrade
root@a20-olinuxino:~# reboot now

# install python
olimex@a20-olinuxino:~$ sudo bash
root@a20-olinuxino:~# apt install htop tig git etckeeper vim curl python3 python3-venv python3-pip
root@a20-olinuxino:~# python3 -m pip install --upgrade pip setuptools wheel
root@a20-olinuxino:~# pip3 install pyA20Lime2 bitarray
root@a20-olinuxino:~# pip3 install  -U pymodbus[twisted] click prompt_toolkit pygments

# enable i2c-1 by running and selecting it (temporary step until fixed by Olimex but not needed for debian 10!)
root@a20-olinuxino:~# olinuxino-overlay

# prove it's enabled by
root@a20-olinuxino:~# i2cdetect -r 1  # should show  0х58 is available

# connect MOD-IO through UEXT cable to UEXT2 at LIME2-SHIELD and power it (12V adapter)

II. finally run this script as root, you should see RELAY1's led showing green color.

III. Integration with Nexedi's toolset for Wendelin 

    # re6snet
    root@a20-olinuxino:~# wget https://deploy.erp5.net/gnet/re6st && bash re6st # get re6stnet token from slapos.vifib.com 
    root@a20-olinuxino:~# echo "default" >> /etc/re6stnet/re6stnet.conf
    root@a20-olinuxino:~# /etc/init.d/re6stnet restart

    # slapos
    root@a20-olinuxino:~# wget https://deploy.erp5.net/slapos && bash slapos # get slapos token from slapos.vifib.com
    
    # OSIE-PLC daemon setup
    ivan@t470:~$ slapos console
    >>> release="https://gitlab+deploy-token-4:pLwtBu8TbusqZDKPUpZA@lab.nexedi.com/nexedi/osie/raw/master/slapos/software/osie-plc/software.cfg"
    >>> supply(release, "COMP-3217")
    

"""

from pyA20Lime2 import i2c
import time

RELAY_MAP = {0: 0x00,
             1: 0x01,
             2: 0x32,
             3: 0x64,
             4: 0x128}

def sendI2Ccommand(code):
    # init I2C
    i2c.init("/dev/i2c-1")
    i2c.open(0x58)
    while 1:
        try:
            i2c.write([0x10, code])
            break
        except:
            print("Failed co sent command.")

    i2c.close()

motor_on = False
valve_on = False

# stop all
sendI2Ccommand(0x00)

while 1:
    command = input("Enter 1 to start motor and 2 to toggle (on/off) air valve. Enter 'q' to quit: ")

    if command == "q":
        break

    try:
        command = int(command)
    except:
        print("Invalid input")
    
    if command == 1:
        # start motor relay
        sendI2Ccommand(0x01)

    elif command == 2:
        #  toggle air valve relay
        if valve_on:
            sendI2Ccommand(0x01)
            valve_on = False
        else:
            sendI2Ccommand(0x03)
            valve_on = True

# stop all
sendI2Ccommand(0x00)
print("Bye!")
