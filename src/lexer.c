#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static Token_Kind token_test_keywords(const Token *identifier) {
    const char *name = identifier->value.as_identifier;
    
    switch (identifier->length) {
    case 2:
        if (strncmp(name, "do", 2) == 0) {
            return Token_Kind__Keyword_Do;
        }
        if (strncmp(name, "if", 2) == 0) {
            return Token_Kind__Keyword_If;
        }
        break;
        
    case 3:
        if (strncmp(name, "asm", 3) == 0) {
            return Token_Kind__Keyword_Asm;
        }
        if (strncmp(name, "for", 3) == 0) {
            return Token_Kind__Keyword_For;
        }
        if (strncmp(name, "int", 3) == 0) {
            return Token_Kind__Keyword_Int;
        }
        break;

    case 4:
        if (strncmp(name, "auto", 4) == 0) {
            return Token_Kind__Keyword_Auto;
        }
        if (strncmp(name, "bool", 4) == 0) {
            return Token_Kind__Keyword_Bool;
        }
        if (strncmp(name, "case", 4) == 0) {
            return Token_Kind__Keyword_Case;
        }
        if (strncmp(name, "char", 4) == 0) {
            return Token_Kind__Keyword_Char;
        }
        if (strncmp(name, "else", 4) == 0) {
            return Token_Kind__Keyword_Else;
        }
        if (strncmp(name, "enum", 4) == 0) {
            return Token_Kind__Keyword_Enum;
        }
        if (strncmp(name, "goto", 4) == 0) {
            return Token_Kind__Keyword_Goto;
        }
        if (strncmp(name, "long", 4) == 0) {
            return Token_Kind__Keyword_Long;
        }
        if (strncmp(name, "true", 4) == 0) {
            return Token_Kind__Keyword_True;
        }
        if (strncmp(name, "void", 4) == 0) {
            return Token_Kind__Keyword_Void;
        }
        break;

    case 5:
        if (strncmp(name, "_Bool", 5) == 0) {
            return Token_Kind__Keyword_Bool;
        }
        if (strncmp(name, "break", 5) == 0) {
            return Token_Kind__Keyword_Break;
        }
        if (strncmp(name, "const", 5) == 0) {
            return Token_Kind__Keyword_Const;
        }
        if (strncmp(name, "false", 5) == 0) {
            return Token_Kind__Keyword_False;
        }
        if (strncmp(name, "float", 5) == 0) {
            return Token_Kind__Keyword_Float;
        }
        if (strncmp(name, "short", 5) == 0) {
            return Token_Kind__Keyword_Short;
        }
        if (strncmp(name, "union", 5) == 0) {
            return Token_Kind__Keyword_Union;
        }
        if (strncmp(name, "while", 5) == 0) {
            return Token_Kind__Keyword_While;
        }
        break;

    case 6:
        if (strncmp(name, "double", 6) == 0) {
            return Token_Kind__Keyword_Double;
        }
        if (strncmp(name, "extern", 6) == 0) {
            return Token_Kind__Keyword_Extern;
        }
        if (strncmp(name, "inline", 6) == 0) {
            return Token_Kind__Keyword_Inline;
        }
        if (strncmp(name, "return", 6) == 0) {
            return Token_Kind__Keyword_Return;
        }
        if (strncmp(name, "signed", 6) == 0) {
            return Token_Kind__Keyword_Signed;
        }
        if (strncmp(name, "sizeof", 6) == 0) {
            return Token_Kind__Keyword_Sizeof;
        }
        if (strncmp(name, "static", 6) == 0) {
            return Token_Kind__Keyword_Static;
        }
        if (strncmp(name, "struct", 6) == 0) {
            return Token_Kind__Keyword_Struct;
        }
        if (strncmp(name, "switch", 6) == 0) {
            return Token_Kind__Keyword_Switch;
        }
        if (strncmp(name, "typeof", 6) == 0) {
            return Token_Kind__Keyword_Typeof;
        }
        break;

    case 7:
        if (strncmp(name, "_Atomic", 7) == 0) {
            return Token_Kind__Keyword_Atomic;
        }
        if (strncmp(name, "_BigInt", 7) == 0) {
            return Token_Kind__Keyword_Bigint;
        }
        if (strncmp(name, "alignas", 7) == 0) {
            return Token_Kind__Keyword_Alignas;
        }
        if (strncmp(name, "alignof", 7) == 0) {
            return Token_Kind__Keyword_Alignof;
        }
        if (strncmp(name, "default", 7) == 0) {
            return Token_Kind__Keyword_Default;
        }
        if (strncmp(name, "nullptr", 7) == 0) {
            return Token_Kind__Keyword_Nullptr;
        }
        if (strncmp(name, "typedef", 7) == 0) {
            return Token_Kind__Keyword_Typedef;
        }
        break;

    case 8:
        if (strncmp(name, "_Alignas", 8) == 0) {
            return Token_Kind__Keyword_Alignas;
        }
        if (strncmp(name, "_Alignof", 8) == 0) {
            return Token_Kind__Keyword_Alignof;
        }
        if (strncmp(name, "_Complex", 8) == 0) {
            return Token_Kind__Keyword_Complex;
        }
        if (strncmp(name, "_Generic", 8) == 0) {
            return Token_Kind__Keyword_Generic;
        }
        if (strncmp(name, "continue", 8) == 0) {
            return Token_Kind__Keyword_Continue;
        }
        if (strncmp(name, "register", 8) == 0) {
            return Token_Kind__Keyword_Register;
        }
        if (strncmp(name, "restrict", 8) == 0) {
            return Token_Kind__Keyword_Restrict;
        }
        if (strncmp(name, "unsigned", 8) == 0) {
            return Token_Kind__Keyword_Unsigned;
        }
        if (strncmp(name, "volatile", 8) == 0) {
            return Token_Kind__Keyword_Volatile;
        }
        break;

    case 9:
        if (strncmp(name, "_Noreturn", 9) == 0) {
            return Token_Kind__Keyword_Noreturn;
        }
        if (strncmp(name, "constexpr", 9) == 0) {
            return Token_Kind__Keyword_Constexpr;
        }
        break;

    case 10:
        if (strncmp(name, "_Decimal32", 10) == 0) {
            return Token_Kind__Keyword_Decimal32;
        }
        if (strncmp(name, "_Decimal64", 10) == 0) {
            return Token_Kind__Keyword_Decimal64;
        }
        if (strncmp(name, "_Imaginary", 10) == 0) {
            return Token_Kind__Keyword_Imaginary;
        }
        break;

    case 11:
        if (strncmp(name, "_Decimal128", 11) == 0) {
            return Token_Kind__Keyword_Decimal128;
        }
        break;

    case 12:
        if (strncmp(name, "thread_local", 12) == 0) {
            return Token_Kind__Keyword_Thread_Local;
        }
        break;
        
    case 13:
        if (strncmp(name, "_Thread_local", 13) == 0) {
            return Token_Kind__Keyword_Thread_Local;
        }
        if (strncmp(name, "static_assert", 13) == 0) {
            return Token_Kind__Keyword_Static_Assert;
        }
        if (strncmp(name, "typeof_unqual", 13) == 0) {
            return Token_Kind__Keyword_Typeof_Unqual;
        }
        break;

    case 14:
        if (strncmp(name, "_Static_assert", 14) == 0) {
            return Token_Kind__Keyword_Static_Assert;
        }
        break;
    }

    return Token_Kind__Identifier;
}

static Lexer_Status lexer_chop_identifier(Lexer *lexer, Token **out_identifier) {
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

    *out_identifier = identifier;
    return Lexer_Status__Token_Found;
}

static Lexer_Status lexer_tokenize_identifier_or_keyword(Lexer *lexer) {
    Token *identifier = NULL;
    Lexer_Status status = lexer_chop_identifier(lexer, &identifier);
    if (status != Lexer_Status__Token_Found) {
        return status;
    }
        
    identifier->kind = token_test_keywords(identifier);

    lexer_accept_token(lexer, identifier);
    
    return Lexer_Status__Token_Found;
}

static Lexer_Status lexer_tokenize_preprocessor_directive(Lexer *lexer) {
    if (lexer->head[0] != '#') {
        return Lexer_Status__Ok;
    }

    ++lexer->head;

    return Lexer_Status__Not_Implemented;
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

static Lexer_Status lexer_tokenize_literal_char(Lexer *lexer) {
    if (lexer->head[0] != '\'') {
        return Lexer_Status__Ok;
    }
    ++lexer->head;

    if (lexer_near_end(lexer, 1)) {
        return Lexer_Status__Unexpected_Character;
    }

    Token *literal = lexer_ready_token(lexer, Token_Kind__Value_Char);

    if (lexer->head[0] == '\\') {
        ++lexer->head;

        if (lexer_near_end(lexer, 1)) {
            return Lexer_Status__Unexpected_Character;
        }
    }

    literal->value.as_char = *lexer->head;
    ++lexer->head;

    if (lexer->head[0] != '\'') {
        return Lexer_Status__Unexpected_Character;
    }
    ++lexer->head;
    
    lexer_accept_token(lexer, literal);

    return Lexer_Status__Token_Found;
}

static Lexer_Status lexer_tokenize_literal(Lexer *lexer) {
    Lexer_Status status;

    if ((status = lexer_tokenize_literal_char(lexer)) != Lexer_Status__Ok) {
        return status;
    }

    return Lexer_Status__Ok;
}

static Lexer_Status lexer_tokenize_next(Lexer *lexer) {
    Lexer_Status status = Lexer_Status__Ok;

    if ((status = lexer_tokenize_preprocessor_directive(lexer)) != Lexer_Status__Ok) {
        return status;
    }
    
    if ((status = lexer_tokenize_identifier_or_keyword(lexer)) != Lexer_Status__Ok) {
        return status;
    }

    if ((status = lexer_tokenize_symbol(lexer)) != Lexer_Status__Ok) {
        return status;
    }

    if ((status = lexer_tokenize_literal(lexer)) != Lexer_Status__Ok) {
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
