MAIN_PROG=hinfosvc
NAME=xnovos13

CC=gcc
CFLAGS= -std=c99 -Wall -Wextra -pedantic -g -fcommon
STR_DEPS= str_obj
.PHONY=clean, zip


$(MAIN_PROG): $(MAIN_PROG).o lib_str_obj.a
	${CC} $^ -o $@

lib_str_obj.a: ${STR_DEPS}.o
	ar -rcs $@ $^

zip:
	zip ${NAME} ${MAIN_PROG}.* ${STR_DEPS}.* Readme.md Makefile

clean:
	rm -f $(MAIN_PROG) *.a *.o
