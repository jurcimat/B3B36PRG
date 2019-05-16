CFLAGS+= -Wall -Werror -std=gnu99 -g
LDFLAGS=-pthread

HW=hw10
BINARIES=hw10-main
all: ${BINARIES}

OBJS=${patsubst %.c,%.o,${wildcard *.c}}

hw10-main: ${OBJS}
	${CC} ${LDFLAGS} ${OBJS} -o $@

${OBJS}: %.o: %.c
	${CC} -c ${CFLAGS} $< -o $@

zip:
	zip ${HW}-brute.zip nucleo/hw10-mbed.cpp ${wildcard *.c} ${wildcard *.h}
	
clean:
	rm -f ${BINARIES} ${OBJS}
	rm -f ${HW}-brute.zip
