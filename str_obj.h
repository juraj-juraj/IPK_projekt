#ifndef __STR_OBJ_LIB
#define __STR_OBJ_LIB 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 


#define INIT_STR_LEN 1000
#define STR_REALLOC_CONST 2

typedef enum string_word{
    Spaces,
    Word,
    End
} fsm_string_t;

typedef struct string_c{
    int length;
    int cursor; //cursor is set on first blank character
    char *string;
} string_t;


int str_init(string_t *to_set);

int str_destruct(string_t *string);

int str_realloc(string_t *string);

int str_append(string_t *string, char *message, int mess_len);

int str_merge(string_t *dest, string_t *src);

int str_clear(string_t *string);

int is_space(char symbol);

int str_print_word(string_t *string);

int str_start_with(string_t *string, char *start, int start_len);

int str_nword(string_t *string);

#endif