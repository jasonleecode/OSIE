#!/bin/bash
# =================================================
#
#        Open62541 installation in Ubuntu 20.04
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

      # Path preparation
      LOCAL_HOME_DIR=/home/${SUDO_USER:-$(whoami)}/
      echo "$LOCAL_HOME_DIR"

      # Build and install open62541
      echo "Building and installing open62541."
      cd $LOCAL_HOME_DIR/
      git clone https://github.com/open62541/open62541.git
      cd open62541/
      git submodule update --init --recursive
      mkdir -p build && cd build
      cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo -DUA_NAMESPACE_ZERO=FULL -DUA_ENABLE_AMALGAMATION=ON ..
      make -j
      make install

      echo "open62541 is installed." 
      echo "Transfer open62541.c and open62541.h to your project location from the buld directory"
      echo "Ex: cp ~/open62541/build/open62541.* ~/osie/coupler/opc-ua-server/"

   else 
      echo "Distro is not supported."
      exit 3
   fi
else
   echo "ERROR: OS is not supported: $OSTYPE"
   exit 4
fi