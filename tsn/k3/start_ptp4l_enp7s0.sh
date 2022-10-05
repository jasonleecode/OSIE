#!/bin/bash
chrt -f 97 ptp4l -H -i enp7s0  --step_threshold=1 -m &> ptp4l_enp7s0.log &

pmc -u -b 0 -i enp7s0 "SET GRANDMASTER_SETTINGS_NP clockClass 248 clockAccuracy 0xfe offsetScaledLogVariance 0xffff currentUtcOffset 37 leap61 0 leap59 0 currentUtcOffsetValid 1 ptpTimescale 1 timeTraceable 1 frequencyTraceable 0 timeSource 0xa0"

chrt -f 95 phc2sys -m -c enp7s0 -s CLOCK_REALTIME --step_threshold=1 -w &> phc2sys_enp7s0.log &
