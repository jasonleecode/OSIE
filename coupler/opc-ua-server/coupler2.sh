#!/bin/bash
COUPLER_SR=.
$COUPLER_SR/server -p 4841 -m 1 -b 1 -i 2 -l 1 -t 1000 -o 5000 -n opc.udp://224.0.0.22:4840/
