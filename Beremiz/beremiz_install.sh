#!/bin/bash
# =================================================
#
#        Beremiz installation in Ubuntu 20.04
#
# =================================================


# run as root
if [[ $EUID -ne 0 ]]; then
   echo 'run this script as sudo !' 1>&2
   exit 1
fi

echo "OSTYPE:"

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
   ls -al /etc/*-release
   cat /etc/arch-release

   # Ubuntu
   if [ -f /etc/os-release ]; then
      . /etc/os-release
      OS_NAME=$NAME
      OS_VER_ID=$VERSION_ID
      echo "System OS_NAME: $OS_NAME"

   else
      echo "tested release files do not exist -> happens with older or unsupported OS versions"
      exit 2
   fi

   echo "Found OS information: $OS_NAME, version: $OS_VER_ID"

   if [[ "$OS_NAME" == *"Ubuntu"* ]]; then
      echo "Ubuntu"
      export DEBIAN_FRONTEND=noninteractive

      # Update system
      # install required packages
      apt -yq update
      apt -yq install python-wxgtk3.0 python-lxml python-is-python2 python-numpy build-essential python-dev-is-python2 mercurial autoconf bison flex python2 virtualenv python-lxml python-numpy libgtk-3-dev libgl1-mesa-dev libglu1-mesa-dev
      pip2 install pyro twisted nevow autobahn lxml opcua

      # Path preparation
      LOCAL_HOME_DIR=/home/${SUDO_USER:-$(whoami)}/
      echo "$LOCAL_HOME_DIR"

      # Build Beremiz
      echo "Building Beremiz."
      cd $LOCAL_HOME_DIR/
      mkdir -p Beremiz
      cd Beremiz/
      hg clone https://hg.beremiz.org/beremiz

      # Build Matiec
      echo "Building Matiec."
      hg clone https://hg.beremiz.org/matiec
      autoreconf -i
      ./configure
      make -j
      pwd

      # Build Modbus
      echo "Building Modbus."  
      hg clone https://hg.beremiz.org/Modbus/
      make


   else 
      echo "Distro not supported."
      exit 3
   fi
else
   echo "ERROR: OS not supported: $OSTYPE"
   exit 4
fi