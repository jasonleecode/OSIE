#!/bin/bash
COUPLER_SR=/opt/slapgrid/016f1c79330dfc29ce1563ab3f5f1a40
export LD_LIBRARY_PATH='$LD_LIBRARY_PATH:/opt/slapgrid/016f1c79330dfc29ce1563ab3f5f1a40/parts/open62541/lib:/opt/slapgrid/016f1c79330dfc29ce1563ab3f5f1a40/parts/mbedtls/lib'
$COUPLER_SR/bin/server -m 1 -b 1 -i 1 -l 2 -t 1000 -o 5000 -n opc.udp://224.0.0.22:4840/
