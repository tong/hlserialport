LBITS := $(shell getconf LONG_BIT)
ARCH ?= $(LBITS)
SRC := serialport.c
CFLAGS := -std=c11 -O3 -Wall -m$(ARCH) -fPIC
LFLAGS := -lhl

all: hdll

serialport.hdll: ${SRC}
	$(CC) -o $@ -shared $(CFLAGS) $(LFLAGS) $(SRC)

hdll: serialport.hdll

clean:
	rm -f serialport.hdll
	rm -f example/*.hl

.PHONY: all hdll clean
