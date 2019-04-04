VALGRIND := "1"
SHELL := /bin/bash
GREP := grep
CC := clang 
CFLAGS+= -pedantic -Wall -g -Werror -Werror=unused-result -lm -std=c99 -O3
MANTESTS := $(shell ls data/man/ | ${GREP} .in | ${GREP} -Po '\d+' | sed ':a;N;$!ba;s/\n/\" \"/g' ; )
OPTESTS := $(shell ls data/opt/ | ${GREP} .in | ${GREP} -Po '\d+' | sed ':a;N;$!ba;s/\n/\" \"/g' ; )
TESTS := $(shell ls data/ | ${GREP} .in | ${GREP} -Po '\d+' | sed ':a;N;$!ba;s/\n/\" \"/g' ; )
# objects := $(patsubst %.c,%.o,$(wildcard *.c))
objects := main.c
TESTOUTPUT = "tests/"
MANTESTOUTPUT = "mantests/"
OPTESTOUTPUT = "optests/"

program: $(objects)
	${CC} ${CFLAGS} -o program $(objects) 

zip:
	@if [ -f "`which zip`" ]; then \
	 zip hw.zip main.c; \
	fi;

basictest: FORCE
	@if ! [ -d "$TESTOUTPUT" ] ; then\
 mkdir -p ${TESTOUTPUT} ; \
 fi;
	@$(foreach var,$(TESTS), ./program < data/pub${var}.in 1> ${TESTOUTPUT}TEST_${var} 2> /dev/null; if [ "`hexdump ${TESTOUTPUT}TEST_${var}`" = "`hexdump data/pub${var}.out`" ] ; then \
printf "TEST ${var} OUT [\033[0;32m OK \033[0m] "; \
else \
printf "\n ----- EXPECTED RESULT ----- \n\n" >> ${TESTOUTPUT}TEST_${var}; \
cat data/pub${var}.out >> ${TESTOUTPUT}TEST_${var}; \
printf "\n--- HEX ---\n" >> ${TESTOUTPUT}TEST_${var}; \
 ./program <data/pub${var}.in 2>/dev/null| hexdump -C >> ${TESTOUTPUT}TEST_${var} ; \
printf "\n--- EXPECTED HEX ---\n" >> ${TESTOUTPUT}TEST_${var}; \
hexdump -C data/pub${var}.out >> ${TESTOUTPUT}TEST_${var}; \
printf "TEST ${var} OUT [\033[0;31mFAIL\033[0m] "; \
fi;\
if  test -f data/pub${var}.err  ; then \
printf  " ------- [ STDERR ] ------\n"  >>${TESTOUTPUT}TEST_${var};\
 if [ "`./program  < data/pub$(var).in 2>&1 >/dev/null | tee -a ${TESTOUTPUT}TEST_${var} | hexdump`" = "`hexdump data/pub${var}.err`" ] ; then \
printf " ERR [ \033[0;32mOK \033[0m]";    \
else  \
printf "\n ----- EXPECTED RESULT ----- \n\n" >> ${TESTOUTPUT}TEST_${var}; \
cat data/pub${var}.err >> ${TESTOUTPUT}TEST_${var}; \
printf "\n--- HEX ---\n" >> ${TESTOUTPUT}TEST_${var}; \
./program < data/pub${var}.in 2>&1 | hexdump -C >> ${TESTOUTPUT}TEST_${var} ; \
printf "\n--- EXPECTED HEX ---\n" >> ${TESTOUTPUT}TEST_${var}; \
hexdump -C data/pub${var}.err >> ${TESTOUTPUT}TEST_${var};\
printf " ERR [\033[0;31mFAIL\033[0m]"; \
 fi; \
 fi; \
 if [ "${VALGRIND}" = "1" ]; then\
 valgrind --leak-check=full --error-exitcode=125 ./program < data/pub${var}.in >/dev/null 2>&1; \
RETURN=$$?;\
printf " VALGRIND";  \
if [ $$RETURN = 125 ] ; then\
 printf  " [\033[0;31mFAIL\033[0m]"; \
 else \
 printf  " [ \033[0;32mOK \033[0m]";  \
 fi;\
 fi; \
./program < data/pub${var}.in >/dev/null 2>&1;\
 RETURN=$$?;\
printf " PROGRAM RETURNED $$RETURN";  \
if  test -f data/pub${var}.code  ; then \
 if [ RETURN = "`cat data/pub${var}.code`" ] ; then\
 printf  " [ \033[0;32mOK \033[0m]";  \
 else \
 printf  " [\033[0;31mFAIL\033[0m]"; \
 fi;\
fi; \
printf "\n" ;)


mantest: FORCE
	@if ! [ -d "$MANTESTOUTPUT" ] ; then\
 mkdir -p ${MANTESTOUTPUT} ; \
 fi;
	@$(foreach var,$(MANTESTS), ./program < data/man/pub${var}-m.in 1> ${MANTESTOUTPUT}TEST_${var} 2> /dev/null; if [ "`hexdump ${MANTESTOUTPUT}TEST_${var}`" = "`hexdump data/man/pub${var}-m.out`" ] ; then \
printf "MANTEST ${var} OUT [\033[0;32m OK \033[0m] "; \
else \
printf "\n ----- EXPECTED RESULT ----- \n\n" >> ${MANTESTOUTPUT}TEST_${var}; \
cat data/man/pub${var}-m.out >> ${MANTESTOUTPUT}TEST_${var}; \
printf "\n--- HEX ---\n" >> ${MANTESTOUTPUT}TEST_${var}; \
 ./program <data/man/pub${var}-m.in 2>/dev/null| hexdump -C >> ${MANTESTOUTPUT}TEST_${var} ; \
printf "\n--- EXPECTED HEX ---\n" >> ${MANTESTOUTPUT}TEST_${var}; \
hexdump -C data/man/pub${var}-m.out >> ${MANTESTOUTPUT}TEST_${var}; \
printf "MANTEST ${var} OUT [\033[0;31mFAIL\033[0m] "; \
fi;\
if  test -f data/man/pub${var}-m.err  ; then \
printf  " ------- [ STDERR ] ------\n"  >>${MANTESTOUTPUT}TEST_${var};\
 if [ "`./program  < data/man/pub${var}-m.in 2>&1 >/dev/null | tee -a ${MANTESTOUTPUT}TEST_${var} | hexdump`" = "`hexdump data/man/pub${var}-m.err`" ] ; then \
printf " ERR [ \033[0;32mOK \033[0m]";    \
else  \
printf "\n ----- EXPECTED RESULT ----- \n\n" >> ${MANTESTOUTPUT}TEST_${var}; \
cat data/man/pub${var}-m.err >> ${MANTESTOUTPUT}TEST_${var}; \
printf "\n--- HEX ---\n" >> ${MANTESTOUTPUT}TEST_${var}; \
./program < data/man/pub${var}-m.in 2>&1 | hexdump -C >> ${MANTESTOUTPUT}TEST_${var} ; \
printf "\n--- EXPECTED HEX ---\n" >> ${MANTESTOUTPUT}TEST_${var}; \
hexdump -C data/man/pub${var}-m.err >> ${MANTESTOUTPUT}TEST_${var};\
printf " ERR [\033[0;31mFAIL\033[0m]"; \
 fi; \
 fi; \
 if [ "${VALGRIND}" = "1" ]; then\
 valgrind --leak-check=full --error-exitcode=125 ./program < data/man/pub${var}-m.in >/dev/null 2>&1; \
RETURN=$$?;\
printf " VALGRIND";  \
if [ $$RETURN = 125 ] ; then\
 printf  " [\033[0;31mFAIL\033[0m]"; \
 else \
 printf  " [ \033[0;32mOK \033[0m]";  \
 fi;\
 fi; \
./program < data/man/pub${var}-m.in >/dev/null 2>&1;\
 RETURN=$$?;\
printf " PROGRAM RETURNED $$RETURN";  \
if  test -f data/man/pub${var}-m.code  ; then \
 if [ RETURN = "`cat data/man/pub${var}-m.code`" ] ; then\
 printf  " [ \033[0;32mOK \033[0m]";  \
 else \
 printf  " [\033[0;31mFAIL\033[0m]"; \
 fi;\
fi; \
printf "\n" ;)


optest: FORCE
	@if ! [ -d "$OPTESTOUTPUT" ] ; then\
 mkdir -p ${OPTESTOUTPUT} ; \
 fi;
	@$(foreach var,$(OPTESTS), ./program -prg-optional < data/opt/pub${var}-o.in 1> ${OPTESTOUTPUT}TEST_${var} 2> /dev/null; if [ "`hexdump ${OPTESTOUTPUT}TEST_${var}`" = "`hexdump data/opt/pub${var}-o.out`" ] ; then \
printf "OPTEST ${var} OUT [\033[0;32m OK \033[0m] "; \
else \
printf "\n ----- EXPECTED RESULT ----- \n\n" >> ${OPTESTOUTPUT}TEST_${var}; \
cat data/opt/pub${var}-o.out >> ${OPTESTOUTPUT}TEST_${var}; \
printf "\n--- HEX ---\n" >> ${OPTESTOUTPUT}TEST_${var}; \
 ./program -prg-optional <data/opt/pub${var}-o.in 2>/dev/null| hexdump -C >> ${OPTESTOUTPUT}TEST_${var} ; \
printf "\n--- EXPECTED HEX ---\n" >> ${OPTESTOUTPUT}TEST_${var}; \
hexdump -C data/opt/pub${var}-o.out >> ${OPTESTOUTPUT}TEST_${var}; \
printf "OPTEST ${var} OUT [\033[0;31mFAIL\033[0m] "; \
fi;\
if  test -f data/opt/pub${var}-o.err  ; then \
printf  " ------- [ STDERR ] ------\n"  >>${OPTESTOUTPUT}TEST_${var};\
 if [ "`./program -prg-optional  < data/opt/pub${var}-o.in 2>&1 >/dev/null | tee -a ${OPTESTOUTPUT}TEST_${var} | hexdump`" = "`hexdump data/opt/pub${var}-o.err`" ] ; then \
printf " ERR [ \033[0;32mOK \033[0m]";    \
else  \
printf "\n ----- EXPECTED RESULT ----- \n\n" >> ${OPTESTOUTPUT}TEST_${var}; \
cat data/opt/pub${var}-o.err >> ${OPTESTOUTPUT}TEST_${var}; \
printf "\n--- HEX ---\n" >> ${OPTESTOUTPUT}TEST_${var}; \
./program -prg-optional < data/opt/pub${var}-o.in 2>&1 | hexdump -C >> ${OPTESTOUTPUT}TEST_${var} ; \
printf "\n--- EXPECTED HEX ---\n" >> ${OPTESTOUTPUT}TEST_${var}; \
hexdump -C data/opt/pub${var}-o.err >> ${OPTESTOUTPUT}TEST_${var};\
printf " ERR [\033[0;31mFAIL\033[0m]"; \
 fi; \
 fi; \
 if [ "${VALGRIND}" = "1" ]; then\
 valgrind --leak-check=full --error-exitcode=125 ./program -prg-optional < data/opt/pub${var}-o.in >/dev/null 2>&1; \
RETURN=$$?;\
printf " VALGRIND";  \
if [ $$RETURN = 125 ] ; then\
 printf  " [\033[0;31mFAIL\033[0m]"; \
 else \
 printf  " [ \033[0;32mOK \033[0m]";  \
 fi;\
 fi; \
./program -prg-optional < data/opt/pub${var}-o.in >/dev/null 2>&1;\
 RETURN=$$?;\
printf " PROGRAM RETURNED $$RETURN";  \
if  test -f data/opt/pub${var}-o.code  ; then \
 if [ RETURN = "`cat data/opt/pub${var}-o.code`" ] ; then\
 printf  " [ \033[0;32mOK \033[0m]";  \
 else \
 printf  " [\033[0;31mFAIL\033[0m]"; \
 fi;\
fi; \
printf "\n" ;)


clean:
	rm -f *.o
	rm -f program
	rm -f hw.zip
	rm -Rf ${TESTOUTPUT}
	rm -Rf ${OPTESTOUTPUT}
	rm -Rf ${MANTESTOUTPUT}

all: clean program tests zip

tests: program program basictest mantest optest

help:
	@echo "Pouziti: make prikaz"
	@echo "prikaz:  help    - napoveda"
	@echo "         tests   - spustit testy"
	@echo "         clean   - smazat binarku"
	@echo "         program - prelozit program"
	@echo "         zip     - vytvorit zip archiv s main.c"
	@echo "         all     - clean - program - test - zip"

FORCE:
