CC=arm-linux-gnueabihf-gcc
CFLAGS= -I $(OPEN62541_SOURCE_HOME) 
LDFLAGS= -L $(OPEN62541_HOME)/lib
OUT_DIR=bin/

server: server.c
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -std=c99 $(OPEN62541_SOURCE_HOME)/open62541.c
	@cp server $(OUT_DIR)
	@rm server
	
clean:
	@rm $(OUT_DIR)server 2>/dev/null || true

.PHONY: clean
