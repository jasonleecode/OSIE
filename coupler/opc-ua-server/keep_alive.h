// global HEART BEATs of coupler
static unsigned int HEART_BEATS = 0;

// the heart beat interval$$
const int DEFAULT_HEART_BEAT_INTERVAL = 250;
static int HEART_BEAT_INTERVAL = DEFAULT_HEART_BEAT_INTERVAL;

// the list of couplers onto which we depend for properly running$
unsigned int HEART_BEAT_ID_LIST[] = {0, 0, 0, 0, 0, 0, 0, 0};

// the interval for publishing messages$
const int PUBLISHING_INTERVAL = 100;

// a hard coded writer group, data set and publisher ID$
// (should be same for publisher / subscriber)$
const int WRITER_GROUP_ID = 100;
const int DATASET_WRITER_ID = 62541;
const int PUBLISHER_ID = 2234;

