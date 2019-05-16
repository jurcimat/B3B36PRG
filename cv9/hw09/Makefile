CFLAGS+= -Wall -Werror -std=gnu99 -g
LDFLAGS=-pthread

HW=hw09
BINARIES=hw09-main
all: $(BINARIES)

OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

hw09-man: hw09-main

hw09-main: ${OBJS}
	${CC} ${LDFLAGS} hw09-main.c prg_serial_nonblock.o -o $@

${OBJS}: %.o: %.c
	${CC} -c ${CFLAGS} ${LDFLAGS} $< -o $@

zip:
	zip ${HW}-brute.zip nucleo/hw09-mbed.cpp hw09-main.c
	
clean:
	rm -f ${BINARIES} ${OBJS}
	rm -f ${HW}-brute.zip
