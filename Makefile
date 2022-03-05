MAIN_PROG=hinfosvc

CC=gcc
CFLAGS= -std=c99 -Wall -Wextra -pedantic -w -g -fcommon
STR_DEPS= str_obj.o
.PHONY=clean


$(MAIN_PROG): $(MAIN_PROG).o lib_str_obj.a
	${CC} $^ -o $@

lib_str_obj.a: ${STR_DEPS}
	ar -rcs $@ $^

clean:
	rm -f $(MAIN_PROG) *.a *.o