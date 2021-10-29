CC=arm-linux-gnueabihf-gcc
CFLAGS=-I.
OUT_DIR=bin/

server: server.c open62541.c
	$(CC) -o $@ $^ $(CFLAGS)
	@cp server $(OUT_DIR)
	@rm server
	
clean:
	@rm $(OUT_DIR)server 2>/dev/null || true

.PHONY: clean