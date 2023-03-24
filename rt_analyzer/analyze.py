f = open("digital.csv", "r")

# assume it's 0 by default even if in measurement file it can start with 1
# then we simple loose few records but that's acceptable.
last_channel0_value = 0
last_channel1_value = 0

last_channel0_timestamp = 0.0
last_channel1_timestamp = 0.0

# keep list of deltas between state change ( 0 -> 1, 1 -> 0)
timestamp_channel0_delta_list = []
timestamp_channel1_delta_list = []

for line in  f.readlines()[1:4000]:
    timestamp, channel0, channel1 = line.split(",")
    timestamp = float(timestamp)
    channel0 = int(channel0)
    channel1 = int(channel1)
    #print timestamp, channel0, channel1

    # first time init (for very first record line)
    if timestamp == 0.0:
        last_channel0_value = channel0
        last_channel1_value = channel1

    # a new measurement if when channel{0, 1} value changes from last know one
    if channel0 != last_channel0_value:
        # calculate delta
        if last_channel0_timestamp == 0.0:
            # first time init
            delta = 0
        else:
            delta = timestamp - last_channel0_timestamp
        last_channel0_timestamp = timestamp
        # but always add delta
        timestamp_channel0_delta_list.append(delta)

    if channel1 != last_channel1_value:
        # calculate delta
        if last_channel1_timestamp == 0.0:
            # first time init
            delta = 0
        else:
            delta = timestamp - last_channel1_timestamp
        last_channel1_timestamp = timestamp
        # but always add delta
        timestamp_channel1_delta_list.append(delta)

    # update for next cycle
    last_channel0_value = channel0
    last_channel1_value = channel1

# XXX: find average, mean, standard deviation, etc on these lists of deltas

# convert to nanoseconds
print "Channel0 deltas (in nanoseconds)", [round(x*1000000, 2) for x in timestamp_channel0_delta_list]
print "\nChannel1 deltas (in nanoseconds)", [round(x*1000000, 2) for x in timestamp_channel1_delta_list]
f.close()