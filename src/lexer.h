#ifndef LEANC_LEXER_H
#define LEANC_LEXER_H

#include <stddef.h>
#include <stdint.h>

#include "arena.h"

typedef struct Lexer {
    const char *begin;
    const char *end;
    const char *head;

    const char *line_begin;
    size_t line;

    Token_Array token_array;
    Arena *string_arena;

    Token *token;
} Lexer;

typedef enum Lexer_Status {
    Lexer_Status__Ok = 0,
    Lexer_Status__Token_Found,
    
    Lexer_Status__Failure,
    Lexer_Status__Memory_Failure,
    Lexer_Status__Not_Implemented,
    Lexer_Status__Unexpected_Character,
} Lexer_Status;

Lexer_Status lexer_create(Lexer *lexer, const char *source, size_t length);
void lexer_destroy(Lexer *lexer);
Lexer_Status lexer_tokenize(Lexer *lexer);

const char *lexer_status_describe(Lexer_Status status);

#endif // LEANC_LEXER_H
