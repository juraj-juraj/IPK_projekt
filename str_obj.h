#ifndef __STR_OBJ_LIB
#define __STR_OBJ_LIB 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 


#define INIT_STR_LEN 32
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

/**
 * @brief Allocates string of initial size 
 * 
 * @param to_set pointer to structure string_t
 * @return int return value. 0 means succes
 */
int str_init(string_t *to_set);

/**
 * @brief Deallocates string
 * 
 * @param string pointer to structure string_t
 * @return int return value, 0 means success
 */
int str_destruct(string_t *string);

/**
 * @brief Reallocates tring if needed. New size will be 2 times the old size
 * 
 * @param string pointer to structure string_t
 * @return int return value, 0 means success
 */
int str_realloc(string_t *string);

/**
 * @brief Appends message to string. Message is copied to the end of string. Cursor is set to end.
 * May reallocate if needed
 * 
 * @param string pointer to structure string_t
 * @param message message to append to end of string
 * @param mess_len length os appended message
 * @return int return value, 0 means success
 */
int str_append(string_t *string, char *message, int mess_len);

/**
 * @brief merges two strings to dest. Src will stay unchanged.
 * 
 * @param dest where will be result stored
 * @param src what to append
 * @return int return value, 0 means success 
 */
int str_merge(string_t *dest, string_t *src);

/**
 * @brief Set string to \0
 * 
 * @param string pointer to structure string_t
 * @return int return value, 0 means success
 */
int str_clear(string_t *string);

/**
 * @brief Checks if submitted symbol is space
 * 
 * @param symbol 
 * @return int return True, if it is space
 */
int is_space(char symbol);

/**
 * @brief prints one word to stdout. Words are separated by spaces
 * 
 * @param string 
 * @return int return value, 0 means success
 */
int str_print_word(string_t *string);

/**
 * @brief Checks if string starting at cursor starts with start variable
 * 
 * @param string pointer to structure string_t
 * @param start test pattern, which is evaulated
 * @param start_len 
 * @return int if string starts with start, returns true
 */
int str_start_with(string_t *string, char *start, int start_len);

/**
 * @brief Moves cursor to beggining of next word. 
 * Words are separated by whitespaces. 
 * 
 * @param string pointer to structure string_t
 * @return int return value, 0 means success
 */
int str_nword(string_t *string);

#endif