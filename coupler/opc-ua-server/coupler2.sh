#!/bin/bash
COUPLER_SR=/opt/slapgrid/834429afc45fed870654dc335be83cd4
$COUPLER_SR/bin/server -p 4841 -m 1 -b 1 -i 2 -l 1,3 -t 100 -o 10000
