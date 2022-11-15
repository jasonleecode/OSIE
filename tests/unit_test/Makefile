CC=gcc
CFLAGS= -l:libopen62541.so -L/usr/local/lib -Wall -Wno-missing-braces -ggdb `pkg-config --cflags criterion` -I /usr/local/include/ -I ~/git/open62541/src/pubsub/ -I ~/git/open62541/deps/
LDFLAGS= `pkg-config --libs criterion` -lmbedcrypto  -lmbedx509
OUT_DIR=build/

all: test_common test_modio_i2c test_keep_alive test_keep_alive_publisher test_keep_alive_subscriber

test_common: test_common.o
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@mv $@ $(OUT_DIR)

test_modio_i2c: test_modio_i2c.o
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@mv $@ $(OUT_DIR)

test_keep_alive: test_keep_alive.o
	@mkdir -p $(OUT_DIR)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
	@mv $@ $(OUT_DIR)

test_keep_alive_publisher: test_keep_alive_publisher.o
	@mkdir -p $(OUT_DIR)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
	@mv $@ $(OUT_DIR)

test_keep_alive_subscriber: test_keep_alive_subscriber.o
	@mkdir -p $(OUT_DIR)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
	@mv $@ $(OUT_DIR)


run: all 
	@${OUT_DIR}/test_common --tap=${OUT_DIR}/test_common.tap
	@${OUT_DIR}/test_modio_i2c --tap=${OUT_DIR}/test_modio_i2c.tap
	@${OUT_DIR}/test_keep_alive --tap=${OUT_DIR}/test_keep_alive.tap
	@${OUT_DIR}/test_keep_alive_publisher --tap=${OUT_DIR}/test_keep_alive_publisher.tap
	@${OUT_DIR}/test_keep_alive_subscriber --tap=${OUT_DIR}/test_keep_alive_subscriber.tap

clean:
	@rm $(OUT_DIR)test_common 2>/dev/null || true
	@rm $(OUT_DIR)test_common.tap 2>/dev/null || true
	@rm $(OUT_DIR)test_modio_i2c 2>/dev/null || true
	@rm $(OUT_DIR)test_modio_i2c.tap 2>/dev/null || true
	@rm $(OUT_DIR)test_keep_alive 2>/dev/null || true
	@rm $(OUT_DIR)test_keep_alive.tap 2>/dev/null || true
	@rm $(OUT_DIR)test_keep_alive_publisher 2>/dev/null || true
	@rm $(OUT_DIR)test_keep_alive_publisher.tap 2>/dev/null || true
	@rm $(OUT_DIR)test_keep_alive_subscriber 2>/dev/null || true
	@rm $(OUT_DIR)test_keep_alive_subscriber.tap 2>/dev/null || true
	@rm *.o 2>/dev/null || true
	

.PHONY: clean debug all
