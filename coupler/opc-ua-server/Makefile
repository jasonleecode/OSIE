CC=$(C_COMPILER)
CFLAGS= -I $(OPEN62541_SOURCE_HOME) 
LDFLAGS= -L $(OPEN62541_HOME)/lib -lmbedcrypto  -lmbedx509
OUT_DIR= $(BINARY_OUT_DIR)

server: server.c
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -std=c99 $(OPEN62541_SOURCE_HOME)/open62541.c

install:
	@cp server $(OUT_DIR)
	@rm server
	
clean:
	@rm $(OUT_DIR)server 2>/dev/null || true

.PHONY: clean
