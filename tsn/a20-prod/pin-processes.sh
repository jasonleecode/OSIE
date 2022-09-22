#!/bin/sh

# Pin ethernet IRQ bottom half to CPU1
echo 2 > /proc/irq/56/smp_affinity

# Get PID for ethernet IRQ
irq56_pid=$(pgrep "irq/56-eth0")

# Pin ethernet IRQ upper half to CPU1
taskset -p 2 $irq56_pid

# Get PID for ksoftirqd1
ksoftirq1_pid=$(pgrep "ksoftirqd/1")

# Set high priority for ksoftirqd/1
chrt -f -p 97 $ksoftirq1_pid
