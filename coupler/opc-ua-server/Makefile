CC=arm-linux-gnueabihf-gcc
CFLAGS= -I $(OPEN62541_HOME)/build 
LDFLAGS= -L $(OPEN62541_HOME)/build/bin
OUT_DIR=bin/

server: server.c
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -std=c99 $(OPEN62541_HOME)/build/open62541.c
	@cp server $(OUT_DIR)
	@rm server
	
clean:
	@rm $(OUT_DIR)server 2>/dev/null || true

.PHONY: clean
