#!/bin/bash
COUPLER_SR=.
$COUPLER_SR/server -m 1 -b 1 -i 1 -l 2 -t 1000 -o 5000 -n opc.udp://224.0.0.22:4840/
