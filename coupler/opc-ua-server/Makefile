CC=gcc
CFLAGS= -I $(OPEN62541_SOURCE_HOME) 
LDFLAGS= -L $(OPEN62541_HOME)/lib
EXTRA_FLAGS=$(C_COMPILER_EXTRA_FLAGS)
OUT_DIR= bin

server: server.c
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -std=c99 $(EXTRA_FLAGS)

install:
	mkdir -p $(OUT_DIR)
	@cp server $(OUT_DIR)/
	@rm server
	
clean:
	@rm $(OUT_DIR)/server 2>/dev/null || true

.PHONY: clean
