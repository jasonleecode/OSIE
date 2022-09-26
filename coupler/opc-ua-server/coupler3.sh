#!/bin/bash
COUPLER_SR=/opt/slapgrid/f552116ea7549aa455a9739e3c361a25
$COUPLER_SR/bin/server -p 4842 -m 1 -b 1 -i 3 -l 1,2 -t 100 -o 10000
