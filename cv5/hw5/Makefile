uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))

-include modules.mk

HW=hw05
TARGET=main

CFLAGS+=-std=c99 -g -pedantic -Wall -Werror
LDFLAGS+=-lm
 
SRC:=$(TARGET).c $(SRC)
SRC+=$(wildcard *.c)
 
OBJS:=$(patsubst %.c,%.o,$(call uniq,$(SRC)))
 
bin: $(TARGET)

$(OBJS): %.o: %.c
	$(CC) -c $< $(CFLAGS) $(CPPFLAGS) -o $@

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@ 

zip:
	zip ${HW}-brute.zip $(TARGET).c

clean:
	$(RM) $(OBJS) $(TARGET)
	$(RM) -f ${HW}-brute.zip

.PHONY: clean zip
