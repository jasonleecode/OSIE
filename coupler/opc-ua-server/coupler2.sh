#!/bin/bash
COUPLER_SR=/opt/slapgrid/6c567f8750e604c805e3a25adbbd0e44
export LD_LIBRARY_PATH='$LD_LIBRARY_PATH:/opt/slapgrid/6c567f8750e604c805e3a25adbbd0e44/parts/open62541/lib:/opt/slapgrid/6c567f8750e604c805e3a25adbbd0e44/parts/mbedtls/lib'

$COUPLER_SR/bin/server -p 4841 -m 1 -b 1 -i 2 -l 1 -t 1000 -o 5000 -n opc.udp://224.0.0.22:4840/
