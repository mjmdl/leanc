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

static Lexer_Status lexer_tokenize_identifier(Lexer *lexer) {
    if (!is_identifier_initial(lexer->head[0])) {
        return Lexer_Status__Ok;
    }

    Token *identifier = lexer_ready_token(lexer, Token_Kind__Identifier);
    if (identifier == NULL) {
        return Lexer_Status__Memory_Failure;
    }

    const char *head = lexer->head;

    do {
        ++lexer->head;
    } while (!lexer_near_end(lexer, 0) && is_identifier_trailing(lexer->head[0]));

    identifier->length = lexer->head - head;
    identifier->value.as_identifier = arena_duplicate_string(lexer->string_arena, head, identifier->length);
    if (identifier->value.as_identifier == NULL) {
        return Lexer_Status__Memory_Failure;
    }

    lexer_accept_token(lexer, identifier);
    
    return Lexer_Status__Token_Found;
}

static Token_Kind lexer_peek_symbol(const Lexer *lexer) {
    switch (*lexer->head) {
    case '+':
        if (!lexer_near_end(lexer, 1)) {
            switch (lexer->head[1]) {
            case '+':
                return Token_Kind__Symbol_Plus_Plus;
            case '=':
                return Token_Kind__Symbol_Plus_Equal;
            default:
                break;
            }
        }
        return Token_Kind__Symbol_Plus;

    case '-':
        if (!lexer_near_end(lexer, 1)) {
            switch (lexer->head[1]) {
            case '-':
                return Token_Kind__Symbol_Minus_Minus;
            case '=':
                return Token_Kind__Symbol_Minus_Equal;
            case '>':
                return Token_Kind__Symbol_Arrow;
            default:
                break;
            }
        }
        return Token_Kind__Symbol_Minus;

    case '*':
        if (!lexer_near_end(lexer, 1) && lexer->head[1] == '=') {
            return Token_Kind__Symbol_Star_Equal;
        }
        return Token_Kind__Symbol_Star;

    case '/':
        if (!lexer_near_end(lexer, 1) && lexer->head[1] == '=') {
            return Token_Kind__Symbol_Slash_Equal;
        }
        return Token_Kind__Symbol_Slash;

    case '%':
        if (!lexer_near_end(lexer, 1) && lexer->head[1] == '=') {
            return Token_Kind__Symbol_Modulo_Equal;
        }
        return Token_Kind__Symbol_Modulo;

    case '.':
        if (
            !lexer_near_end(lexer, 2) &&
            lexer->head[1] == '.' && lexer->head[2] == '.'
        ) {
            return Token_Kind__Symbol_Ellipsis;
        }
        return Token_Kind__Symbol_Dot;

    case '=':
        if (!lexer_near_end(lexer, 1) && lexer->head[1] == '=') {
            return Token_Kind__Symbol_Equal_Equal;
        }
        return Token_Kind__Symbol_Equal;

    case '>':
        if (!lexer_near_end(lexer, 1)) {
            if (lexer->head[1] == '=') {
                return Token_Kind__Symbol_Greater_Equal;
            }
            if (lexer->head[1] == '>') {
                if (!lexer_near_end(lexer, 2) && lexer->head[2] == '=') {
                    return Token_Kind__Symbol_Shift_Right_Equal;
                }
                return Token_Kind__Symbol_Shift_Right;
            }
        }
        return Token_Kind__Symbol_Greater;

    case '<':
        if (!lexer_near_end(lexer, 1)) {
            if (lexer->head[1] == '=') {
                return Token_Kind__Symbol_Less_Equal;
            }
            if (lexer->head[1] == '<') {
                if (!lexer_near_end(lexer, 2) && lexer->head[2] == '=') {
                    return Token_Kind__Symbol_Shift_Left_Equal;
                }
                return Token_Kind__Symbol_Shift_Left;
            }
        }
        return Token_Kind__Symbol_Less;

    case '!':
        if (!lexer_near_end(lexer, 1) && lexer->head[1] == '=') {
            return Token_Kind__Symbol_Not_Equal;
        }
        return Token_Kind__Symbol_Not;

    case '&':
        if (!lexer_near_end(lexer, 1)) {
            if (lexer->head[1] == '&') {
                return Token_Kind__Symbol_And_And;
            }
            if (lexer->head[1] == '=') {
                return Token_Kind__Symbol_And_Equal;
            }
        }
        return Token_Kind__Symbol_And;

    case '|':
        if (!lexer_near_end(lexer, 1)) {
            if (lexer->head[1] == '|') {
                return Token_Kind__Symbol_Or_Or;
            }
            if (lexer->head[1] == '=') {
                return Token_Kind__Symbol_Or_Equal;
            }
        }
        return Token_Kind__Symbol_Or;

    case '^':
        if (!lexer_near_end(lexer, 1) && lexer->head[1] == '=') {
            return Token_Kind__Symbol_Caret_Equal;
        }
        return Token_Kind__Symbol_Caret;

    case '~':
        return Token_Kind__Symbol_Tilde;
    case ':':
        return Token_Kind__Symbol_Colon;
    case ',':
        return Token_Kind__Symbol_Comma;
    case '{':
        return Token_Kind__Symbol_Curly_Left;
    case '}':
        return Token_Kind__Symbol_Curly_Right;
    case '(':
        return Token_Kind__Symbol_Paren_Left;
    case ')':
        return Token_Kind__Symbol_Paren_Right;
    case '[':
        return Token_Kind__Symbol_Square_Left;
    case ']':
        return Token_Kind__Symbol_Square_Right;
    case ';':
        return Token_Kind__Symbol_Semicolon;

    default:
        break;
    }

    return Token_Kind__None;
}

static Lexer_Status lexer_tokenize_symbol(Lexer *lexer) {
    Token_Kind kind = lexer_peek_symbol(lexer);
    if (kind == Token_Kind__None) {
        return Lexer_Status__Ok;
    }

    Token *symbol = lexer_ready_token(lexer, kind);
    if (symbol == NULL) {
        return Lexer_Status__Memory_Failure;
    }

    symbol->length = token_kind_length(kind);
    lexer->head += symbol->length;

    lexer_accept_token(lexer, symbol);
    
    return Lexer_Status__Token_Found;
}

static Lexer_Status lexer_tokenize_next(Lexer *lexer) {
    Lexer_Status status = Lexer_Status__Ok;
    
    if ((status = lexer_tokenize_identifier(lexer)) != Lexer_Status__Ok) {
        return status;
    }

    if ((status = lexer_tokenize_symbol(lexer)) != Lexer_Status__Ok) {
        return status;
    }

    ++lexer->head;
    return Lexer_Status__Ok;
    
    //return Lexer_Status__Not_Implemented;
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
