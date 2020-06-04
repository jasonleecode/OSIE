#!/usr/bin/python3
"""
This is an example of connecting LIME2 + LIME2-SHIELD + MOD-IO (over UEXT) and controlling a relay (on / off).


I. prepare lime2 + lime2's shield + MOD-IO

# install packages
root@t470:~/tmp$ apt install minicom

ivan@t470:~/tmp$ wget http://images.olimex.com/release/a20/A20-OLinuXino-bionic-minimal-20200601-122736.img.7z
ivan@t470:~/tmp$ 7z e A20-OLinuXino-bionic-minimal-20200601-122736.img.7z 
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
ivan@t470:~/tmp$ minicom -D /dev/ttyACM0

# username and password are olimex / olimex 

# put wifi dongle (or ethernet cable) but get an ipV4 address to ssh on

ssh olimex@<ipv4_address> (password: olimex)

# update software
root@a20-olinuxino:~# apt update
root@a20-olinuxino:~# apt upgrade
root@a20-olinuxino:~# reboot now

# XXX: had to set DNS in /etc/resolv.conf to 8.8.8.8 instead of 127.0.0.57

# install python
olimex@a20-olinuxino:~$ sudo bash
root@a20-olinuxino:~# apt install python3
root@a20-olinuxino:~# apt install python3-venv python3-pip
root@a20-olinuxino:~# python3 -m pip install --upgrade pip setuptools wheel 
root@a20-olinuxino:~# pip3 install pyA20Lime2

# enable i2c-1 by running and selecting it (temporary step until fixed by Olimex)
root@a20-olinuxino:~# olinuxino-overlay 

# prove it's enabled by
root@a20-olinuxino:~# i2cdetect -r 1  # should show  0х58 is available

# connect MOD-IO through UEXT cable to UEXT2 at LIME2-SHIELD and power it (12V adapter)


II. finally run this script as root, you should see RELAY1's led showing green color.

"""

from pyA20Lime2 import i2c
i2c.init("/dev/i2c-1")
i2c.open(0x58)

# switch ON RELAY1
i2c.write([0x10, 0x01])

# switch OFF RELAY1
i2c.write([0x10, 0x00])

