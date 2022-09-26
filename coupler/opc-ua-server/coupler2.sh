#!/bin/bash
COUPLER_SR=/opt/slapgrid/f552116ea7549aa455a9739e3c361a25
$COUPLER_SR/bin/server -p 4841 -m 1 -b 1 -i 2 -l 1,3 -t 100 -o 10000
