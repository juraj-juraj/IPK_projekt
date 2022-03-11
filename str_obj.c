/**
 * @brief Implementation of library for working with strings. 
 * @author Juraj Novosád(xnovos13)
 * @file str_obj.c
 */

#include "str_obj.h"


int str_init(string_t *to_set){
    if((to_set->string = calloc(sizeof(char), INIT_STR_LEN)) == NULL){
        return EXIT_FAILURE;
    }
    to_set->cursor = 0;
    to_set->length = INIT_STR_LEN;
    return EXIT_SUCCESS;
}

int str_destruct(string_t *string){
    free(string->string);
    string->cursor = 0;
    string->length = 0;
    return EXIT_SUCCESS;
}

int str_realloc(string_t *string){
    string->length *= STR_REALLOC_CONST;
    string->string = realloc(string->string, string->length);
    if(string->string == NULL) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int str_append(string_t *string, char *message, int mess_len){
    if((string->cursor  + mess_len) >= string->length){
        if(str_realloc(string)) return EXIT_FAILURE;
    }
    strcpy(&string->string[string->cursor], message);
    string->cursor += mess_len;
    return EXIT_SUCCESS;
}

int str_merge(string_t *dest, string_t *src){
    if(str_append(dest, src->string, src->cursor)) return EXIT_FAILURE;
    str_destruct(src);
    return EXIT_SUCCESS;
}

int str_clear(string_t *string){
    memset(string->string, 0, string->cursor);
    string->cursor = 0;
    return EXIT_SUCCESS;
}

int is_space(char symbol){
    return (symbol == ' ' || symbol == '\n' || symbol == '\r');
}

int str_print_word(string_t *string){
    fsm_string_t state = Spaces;
    for(int i = string->cursor; i < string->length; i++){
        switch(state){
            case Spaces:
                if(is_space(string->string[i])){
                    break;
                }
                state = Word;
                //falltrough
            case Word:
                if(is_space(string->string[i])){
                    putchar('\n');
                    return EXIT_SUCCESS;
                }
                putchar(string->string[i]);
                break;
        }
    }
    return EXIT_FAILURE;
}

int str_start_with(string_t *string, char *start, int start_len){
    if(start_len > strlen(string->string)){
        return 0;
    }
    for(int i=0; i < start_len ; i++){
        if(string->string[string->cursor + i] != start[i]){
            return 0;
        }
    }
    return 1;
}

int str_nword(string_t *string){
    fsm_string_t state = Spaces;
    int done = 0;
    for(int i = string->cursor; i < string->length; i++){
        if(i == string->length -1){
            return EXIT_FAILURE;
        }
        switch (state){
            case Spaces:
                if(is_space(string->string[i])){
                    state = End;
                }
                else {
                    state = Word;
                }
                break; 
            case Word:
                if(is_space(string->string[i])){
                    state = End;
                }     
                break; 
            case End:
                if(!is_space(string->string[i])){
                    string->cursor = i;
                    done = 1; 
                }
                break;
        }
        if(done) break;
        
    }
    return EXIT_SUCCESS;
}
