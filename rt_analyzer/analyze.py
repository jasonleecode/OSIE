"""
    Parse logical analyzer logs. Used together with test_latency.c as generator.
    Usage: python3 analyze.py digital.csv
"""
import sys
import statistics

file_name = sys.argv[1]
f = open(file_name, "r")

# assume it's 0 by default even if in measurement file it can start with 1
# then we simple loose few records but that's acceptable.
last_channel0_value = 0
last_channel1_value = 0

last_channel0_timestamp = 0.0
last_channel1_timestamp = 0.0

# keep list of deltas between state change ( 0 -> 1, 1 -> 0)
timestamp_channel0_delta_list = []
timestamp_channel1_delta_list = []

lines_list = f.readlines()
f.close()
for line in lines_list[1:]:
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
        # but always add delta if not 0
        if delta > 0:
          timestamp_channel0_delta_list.append(delta)

    if channel1 != last_channel1_value:
        # calculate delta
        if last_channel1_timestamp == 0.0:
            # first time init
            delta = 0
        else:
            delta = timestamp - last_channel1_timestamp
        last_channel1_timestamp = timestamp
        # but always add delta if not 0
        if delta > 0:
          timestamp_channel1_delta_list.append(delta)

    # update for next cycle
    last_channel0_value = channel0
    last_channel1_value = channel1

# find average, mean, standard deviation, etc on these lists of deltas
# as all data comes in seconds convert to milli seconds by multiplying by denominator
round_base = 5
denominator = 1000

# convert lists from seconds -> milli seconds
timestamp_channel0_delta_list = [x * denominator for x in timestamp_channel0_delta_list]
timestamp_channel1_delta_list = [x * denominator for x in timestamp_channel1_delta_list]

# calculate it
channel0_mean = statistics.mean(timestamp_channel0_delta_list)
channel0_median =statistics.median(timestamp_channel0_delta_list)
channel0_stdev = statistics.stdev(timestamp_channel0_delta_list)
channel0_stdev_percentile = (channel0_stdev * 100) / channel0_median
try:
  channel0_mode = statistics.mode(timestamp_channel0_delta_list)
except statistics.StatisticsError:
  channel0_mode = None
channel0_min = min(timestamp_channel0_delta_list)
channel0_max = max(timestamp_channel0_delta_list)

channel1_mean = statistics.mean(timestamp_channel1_delta_list)
channel1_median = statistics.median(timestamp_channel1_delta_list)
channel1_stdev = statistics.stdev(timestamp_channel1_delta_list)
channel1_stdev_percentile = (channel1_stdev * 100) / channel1_median
try:
  channel1_mode = statistics.mode(timestamp_channel1_delta_list)
except statistics.StatisticsError:
  channel1_mode = None
channel1_min = min(timestamp_channel1_delta_list)
channel1_max = max(timestamp_channel1_delta_list)

# save only Durations for further analyses
channel_0_text = "\n".join([str(x) for x in timestamp_channel0_delta_list])
channel_1_text = "\n".join([str(x) for x in timestamp_channel1_delta_list])
f = open("channel0_duration.txt", "w")
f.write(channel_0_text)
f.close()
f = open("channel1_duration.txt", "w")
f.write(channel_1_text)
f.close()

stop_time = lines_list[-1].split(",")[0]
print("Timestamp records = ", len(lines_list))
print("Duration (seconds) = ", stop_time)

print("Channel0 (in milli seconds):")
print("\tMean =   ", round(channel0_mean, round_base))
print("\tMedian = ", round(channel0_median, round_base))
print("\tMin =    ", round(channel0_min, round_base))
print("\tMax =    ", round(channel0_max, round_base))
print("\tStandart deviation = ", round(channel0_stdev, round_base))
print("\tStandart deviation (%) = ", round(channel0_stdev_percentile, round_base))
print("\tMode (most occurencies) = ", round(channel0_mode, round_base))

print("\nChannel1 (in milli seconds):")
print("\tMean =   ", round(channel1_mean, round_base))
print("\tMedian = ", round(channel1_median, round_base))
print("\tMin =    ", round(channel1_min, round_base))
print("\tMax =    ", round(channel1_max, round_base))
print("\tStandart deviation = ", round(channel1_stdev, round_base))
print("\tStandart deviation (%) = ", round(channel1_stdev_percentile, round_base))
print("\tMode (most occurencies) = ", round(channel1_mode, round_base))


