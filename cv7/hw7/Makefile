CFLAGS+= -Wall -Werror -std=c99 -g

HW=hw07

BINARIES=hw07-test hw07-testopt-int hw07-testopt-str hw07-testopt-stc
all: $(BINARIES)

hw07-man: hw07-test

hw07-opt: hw07-testopt-int hw07-testopt-str hw07-testopt-stc

hw07-test: hw07-test.c linked_list.o
	$(CC) $(CFLAGS) hw07-test.c linked_list.o -o $@

hw07-testopt-int: hw07-testopt-int.c queue.o
	$(CC) $(CFLAGS) hw07-testopt-int.c queue.o -o $@

hw07-testopt-str: hw07-testopt-str.c queue.o
	$(CC) $(CFLAGS) hw07-testopt-str.c queue.o -o $@

hw07-testopt-stc: hw07-testopt-stc.c queue.o
	$(CC) $(CFLAGS) hw07-testopt-stc.c queue.o -o $@

linked_list.o: linked_list.c 
	$(CC) -c $(CFLAGS) $< -o $@

queue.o: queue.c 
	$(CC) -c $(CFLAGS) $< -o $@
	
zip:
	zip $(HW)-brute.zip linked_list.o queue.c
	
clean:
	rm -f *.o $(BINARIES)
	rm -f $(HW)-brute.zip
