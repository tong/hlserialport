LBITS := $(shell getconf LONG_BIT)
ARCH ?= $(LBITS)
CFLAGS = -std=c11 -O3 -Wall -m$(ARCH) -fPIC
LIBFLAGS = -L. -lhl
OBJ = serialport.o

all: serialport.hdll

serialport.hdll: ${OBJ}
	${CC} ${CFLAGS} -shared -o $@ ${OBJ} ${LIBFLAGS} -L. -lhl

clean:
	rm -f serialport.hdll ${OBJ}
