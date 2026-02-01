#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

static bool is_identifier_initial(char rune) {
    return isalpha(rune) || rune == '_';
}

static bool is_identifier_trailing(char rune) {
    return isalnum(rune) || rune == '_';
}

static Token *lexer_ready_token(Lexer *lexer, Token_Kind kind) {
    if (lexer->token == NULL) {
        lexer->token = token_array_push(&lexer->token_array);

        if (lexer->token == NULL) {
            return NULL;
        }
    }

    lexer->token->kind = kind;
    lexer->token->value.as_char = 0;
    lexer->token->position = lexer->head - lexer->begin;
    lexer->token->line = lexer->line;
    lexer->token->column = lexer->head - lexer->line_begin;
    lexer->token->length = 0;

    return lexer->token;
}

static void lexer_accept_token(Lexer *lexer, const Token *token) {
    assert(token == lexer->token);

    lexer->token = NULL;
}

static bool lexer_near_end(const Lexer *lexer, size_t offset) {
    return (lexer->head + offset) >= lexer->end;
}

static void lexer_skip_whitespace(Lexer *lexer) {
    while (!lexer_near_end(lexer, 0) && isspace(lexer->head[0])) {
        ++lexer->head;
    }
}

Lexer_Status lexer_create(Lexer *lexer, const char *source, size_t length) {
    lexer_destroy(lexer);

    Token_Array token_array = {0};
    if (token_array_create(&token_array, 256) != 0) {
        return Lexer_Status__Memory_Failure;
    }

    Arena *string_arena = arena_create(4 * 1024);
    if (string_arena == NULL) {
        token_array_destroy(&token_array);
        return Lexer_Status__Memory_Failure;
    }

    lexer->begin = source;
    lexer->end = source + length;
    lexer->head = source;
    lexer->line_begin = source;
    lexer->line = 0;
    lexer->token_array = token_array;
    lexer->string_arena = string_arena;
    lexer->token = NULL;

    return Lexer_Status__Ok;
}

void lexer_destroy(Lexer *lexer) {
    token_array_destroy(&lexer->token_array);

    if (lexer->string_arena != NULL) {
        arena_destroy(lexer->string_arena);
        lexer->string_arena = NULL;
    }
}

static Lexer_Status lexer_tokenize_next(Lexer *lexer) {
    (void)lexer;
    return Lexer_Status__Not_Implemented;
}

Lexer_Status lexer_tokenize(Lexer *lexer) {
    for (;;) {
        lexer_skip_whitespace(lexer);

        if (lexer_near_end(lexer, 0)) {
            break;
        }

        Lexer_Status status = lexer_tokenize_next(lexer);
        if (status >= Lexer_Status__Failure) {
            return status;
        }
    }

    return Lexer_Status__Ok;
}

const char *lexer_status_describe(Lexer_Status status) {
    switch (status) {
    case Lexer_Status__Ok:
        return "OK";
    case Lexer_Status__Token_Found:
        return "Token Found";
    case Lexer_Status__Failure:
        return "Failure";
    case Lexer_Status__Memory_Failure:
        return "Memory Failure";
    case Lexer_Status__Not_Implemented:
        return "Not Implemented";
    case Lexer_Status__Unexpected_Character:
        return "Unexpected Character";
    }
    assert(0);
}
