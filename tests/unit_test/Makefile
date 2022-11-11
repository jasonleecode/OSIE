CFLAGS= -Wall -Wno-missing-braces -ggdb `pkg-config --cflags criterion`
LDFLAGS= `pkg-config --libs criterion` -L $(OPEN62541_HOME)/lib
LIB_DIR=$(SRC_DIR)te
OUT_DIR=build/

all: test_common test_modio_i2c test_keep_alive

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
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@mv $@ $(OUT_DIR)


run: all 
	@${OUT_DIR}/test_common --tap=${OUT_DIR}/test_common.tap
	@${OUT_DIR}/test_modio_i2c --tap=${OUT_DIR}/test_modio_i2c.tap
	@${OUT_DIR}/test_keep_alive --tap=${OUT_DIR}/test_keep_alive.tap
	

clean:
	@rm $(OUT_DIR)test_common 2>/dev/null || true
	@rm $(OUT_DIR)test_common.tap 2>/dev/null || true
	@rm $(OUT_DIR)test_modio_i2c 2>/dev/null || true
	@rm $(OUT_DIR)test_modio_i2c.tap 2>/dev/null || true
	@rm $(OUT_DIR)test_keep_alive 2>/dev/null || true
	@rm $(OUT_DIR)test_keep_alive.tap 2>/dev/null || true
	@rm *.o 2>/dev/null || true
	

.PHONY: clean debug all
