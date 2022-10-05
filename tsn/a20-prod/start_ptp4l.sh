#!/bin/bash
chrt -f 97 ptp4l -s -S -i eth0 --step_threshold=1 -m &> ptp4l_eth0.log &
