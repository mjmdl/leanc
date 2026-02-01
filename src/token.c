#include <stdlib.h>
#include <stdio.h>

#include "token.h"

typedef struct Token_Kind_Info {
    const char *name;
    size_t length;
} Token_Kind_Info;

static const Token_Kind_Info token_kind_table[Token_Kind__Enum_Count] = {
    [Token_Kind__None] = {.name = "None", .length = 0},

    [Token_Kind__Identifier] = {.name = "Identifier", .length = 0},

    [Token_Kind__Value_Char] = {.name = "Value::Char", .length = 0},
    [Token_Kind__Value_Float64] = {.name = "Value::Float64", .length = 0},
    [Token_Kind__Value_Int64] = {.name = "Value::Int64", .length = 0},
    [Token_Kind__Value_String] = {.name = "Value::String", .length = 0},
    [Token_Kind__Value_Uint64] = {.name = "Value::Uint64", .length = 0},

    [Token_Kind__Keyword_Alignas] = {.name = "Keyword::Alignas", .length = 7},
    [Token_Kind__Keyword_Alignof] = {.name = "Keyword::Alignof", .length = 7},
    [Token_Kind__Keyword_Asm] = {.name = "Keyword::Asm", .length = 3},
    [Token_Kind__Keyword_Atomic] = {.name = "Keyword::Atomic", .length = 7},
    [Token_Kind__Keyword_Auto] = {.name = "Keyword::Auto", .length = 4},
    [Token_Kind__Keyword_Bigint] = {.name = "Keyword::Bigint", .length = 6},
    [Token_Kind__Keyword_Bool] = {.name = "Keyword::Bool", .length = 4},
    [Token_Kind__Keyword_Break] = {.name = "Keyword::Break", .length = 5},
    [Token_Kind__Keyword_Case] = {.name = "Keyword::Case", .length = 4},
    [Token_Kind__Keyword_Char] = {.name = "Keyword::Char", .length = 4},
    [Token_Kind__Keyword_Complex] = {.name = "Keyword::Complex", .length = 7},
    [Token_Kind__Keyword_Const] = {.name = "Keyword::Const", .length = 5},
    [Token_Kind__Keyword_Constexpr] = {.name = "Keyword::Constexpr", .length = 9},
    [Token_Kind__Keyword_Continue] = {.name = "Keyword::Continue", .length = 8},
    [Token_Kind__Keyword_Decimal128] = {.name = "Keyword::Decimal128", .length = 10},
    [Token_Kind__Keyword_Decimal32] = {.name = "Keyword::Decimal32", .length = 8},
    [Token_Kind__Keyword_Decimal64] = {.name = "Keyword::Decimal64", .length = 8},
    [Token_Kind__Keyword_Default] = {.name = "Keyword::Default", .length = 7},
    [Token_Kind__Keyword_Do] = {.name = "Keyword::Do", .length = 2},
    [Token_Kind__Keyword_Double] = {.name = "Keyword::Double", .length = 6},
    [Token_Kind__Keyword_Else] = {.name = "Keyword::Else", .length = 4},
    [Token_Kind__Keyword_Enum] = {.name = "Keyword::Enum", .length = 4},
    [Token_Kind__Keyword_Extern] = {.name = "Keyword::Extern", .length = 6},
    [Token_Kind__Keyword_False] = {.name = "Keyword::False", .length = 5},
    [Token_Kind__Keyword_Float] = {.name = "Keyword::Float", .length = 5},
    [Token_Kind__Keyword_For] = {.name = "Keyword::For", .length = 3},
    [Token_Kind__Keyword_Generic] = {.name = "Keyword::Generic", .length = 7},
    [Token_Kind__Keyword_Goto] = {.name = "Keyword::Goto", .length = 4},
    [Token_Kind__Keyword_If] = {.name = "Keyword::If", .length = 2},
    [Token_Kind__Keyword_Imaginary] = {.name = "Keyword::Imaginary", .length = 9},
    [Token_Kind__Keyword_Inline] = {.name = "Keyword::Inline", .length = 6},
    [Token_Kind__Keyword_Int] = {.name = "Keyword::Int", .length = 3},
    [Token_Kind__Keyword_Long] = {.name = "Keyword::Long", .length = 4},
    [Token_Kind__Keyword_Noreturn] = {.name = "Keyword::Noreturn", .length = 8},
    [Token_Kind__Keyword_Nullptr] = {.name = "Keyword::Nullptr", .length = 7},
    [Token_Kind__Keyword_Register] = {.name = "Keyword::Register", .length = 8},
    [Token_Kind__Keyword_Restrict] = {.name = "Keyword::Restrict", .length = 8},
    [Token_Kind__Keyword_Return] = {.name = "Keyword::Return", .length = 6},
    [Token_Kind__Keyword_Short] = {.name = "Keyword::Short", .length = 5},
    [Token_Kind__Keyword_Signed] = {.name = "Keyword::Signed", .length = 6},
    [Token_Kind__Keyword_Sizeof] = {.name = "Keyword::Sizeof", .length = 6},
    [Token_Kind__Keyword_Static] = {.name = "Keyword::Static", .length = 6},
    [Token_Kind__Keyword_Static_Assert] = {.name = "Keyword::Static_Assert", .length = 13},
    [Token_Kind__Keyword_Struct] = {.name = "Keyword::Struct", .length = 6},
    [Token_Kind__Keyword_Switch] = {.name = "Keyword::Switch", .length = 6},
    [Token_Kind__Keyword_Thread_Local] = {.name = "Keyword::Thread_Local", .length = 12},
    [Token_Kind__Keyword_True] = {.name = "Keyword::True", .length = 4},
    [Token_Kind__Keyword_Typedef] = {.name = "Keyword::Typedef", .length = 7},
    [Token_Kind__Keyword_Typeof] = {.name = "Keyword::Typeof", .length = 6},
    [Token_Kind__Keyword_Typeof_Unqual] = {.name = "Keyword::Typeof_Unqual", .length = 12},
    [Token_Kind__Keyword_Union] = {.name = "Keyword::Union", .length = 5},
    [Token_Kind__Keyword_Unsigned] = {.name = "Keyword::Unsigned", .length = 8},
    [Token_Kind__Keyword_Void] = {.name = "Keyword::Void", .length = 4},
    [Token_Kind__Keyword_Volatile] = {.name = "Keyword::Volatile", .length = 8},
    [Token_Kind__Keyword_While] = {.name = "Keyword::While", .length = 5},

    [Token_Kind__Symbol_And] = {.name = "Symbol::And", .length = 1},
    [Token_Kind__Symbol_And_And] = {.name = "Symbol::And_And", .length = 2},
    [Token_Kind__Symbol_And_Equal] = {.name = "Symbol::And_Equal", .length = 2},
    [Token_Kind__Symbol_Arrow] = {.name = "Symbol::Arrow", .length = 2},
    [Token_Kind__Symbol_Caret] = {.name = "Symbol::Caret", .length = 1},
    [Token_Kind__Symbol_Caret_Equal] = {.name = "Symbol::Caret_Equal", .length = 2},
    [Token_Kind__Symbol_Colon] = {.name = "Symbol::Colon", .length = 1},
    [Token_Kind__Symbol_Comma] = {.name = "Symbol::Comma", .length = 1},
    [Token_Kind__Symbol_Curly_Left] = {.name = "Symbol::Curly_Left", .length = 1},
    [Token_Kind__Symbol_Curly_Right] = {.name = "Symbol::Curly_Right", .length = 1},
    [Token_Kind__Symbol_Dot] = {.name = "Symbol::Dot", .length = 1},
    [Token_Kind__Symbol_Ellipsis] = {.name = "Symbol::Ellipsis", .length = 3},
    [Token_Kind__Symbol_Equal] = {.name = "Symbol::Equal", .length = 1},
    [Token_Kind__Symbol_Equal_Equal] = {.name = "Symbol::Equal_Equal", .length = 2},
    [Token_Kind__Symbol_Greater] = {.name = "Symbol::Greater", .length = 1},
    [Token_Kind__Symbol_Greater_Equal] = {.name = "Symbol::Greater_Equal", .length = 2},
    [Token_Kind__Symbol_Less] = {.name = "Symbol::Less", .length = 1},
    [Token_Kind__Symbol_Less_Equal] = {.name = "Symbol::Less_Equal", .length = 2},
    [Token_Kind__Symbol_Minus] = {.name = "Symbol::Minus", .length = 1},
    [Token_Kind__Symbol_Minus_Equal] = {.name = "Symbol::Minus_Equal", .length = 2},
    [Token_Kind__Symbol_Minus_Minus] = {.name = "Symbol::Minus_Minus", .length = 2},
    [Token_Kind__Symbol_Modulo] = {.name = "Symbol::Modulo", .length = 1},
    [Token_Kind__Symbol_Modulo_Equal] = {.name = "Symbol::Modulo_Equal", .length = 2},
    [Token_Kind__Symbol_Not] = {.name = "Symbol::Not", .length = 1},
    [Token_Kind__Symbol_Not_Equal] = {.name = "Symbol::Not_Equal", .length = 2},
    [Token_Kind__Symbol_Or] = {.name = "Symbol::Or", .length = 1},
    [Token_Kind__Symbol_Or_Equal] = {.name = "Symbol::Or_Equal", .length = 2},
    [Token_Kind__Symbol_Or_Or] = {.name = "Symbol::Or_Or", .length = 2},
    [Token_Kind__Symbol_Paren_Left] = {.name = "Symbol::Paren_Left", .length = 1},
    [Token_Kind__Symbol_Paren_Right] = {.name = "Symbol::Paren_Right", .length = 1},
    [Token_Kind__Symbol_Plus] = {.name = "Symbol::Plus", .length = 1},
    [Token_Kind__Symbol_Plus_Equal] = {.name = "Symbol::Plus_Equal", .length = 2},
    [Token_Kind__Symbol_Plus_Plus] = {.name = "Symbol::Plus_Plus", .length = 2},
    [Token_Kind__Symbol_Question] = {.name = "Symbol::Question", .length = 1},
    [Token_Kind__Symbol_Semicolon] = {.name = "Symbol::Semicolon", .length = 1},
    [Token_Kind__Symbol_Shift_Left] = {.name = "Symbol::Shift_Left", .length = 2},
    [Token_Kind__Symbol_Shift_Left_Equal] = {.name = "Symbol::Shift_Left_Equal", .length = 3},
    [Token_Kind__Symbol_Shift_Right] = {.name = "Symbol::Shift_Right", .length = 2},
    [Token_Kind__Symbol_Shift_Right_Equal] = {.name = "Symbol::Shift_Right_Equal", .length = 3},
    [Token_Kind__Symbol_Slash] = {.name = "Symbol::Slash", .length = 1},
    [Token_Kind__Symbol_Slash_Equal] = {.name = "Symbol::Slash_Equal", .length = 2},
    [Token_Kind__Symbol_Square_Left] = {.name = "Symbol::Square_Left", .length = 1},
    [Token_Kind__Symbol_Square_Right] = {.name = "Symbol::Square_Right", .length = 1},
    [Token_Kind__Symbol_Star] = {.name = "Symbol::Star", .length = 1},
    [Token_Kind__Symbol_Star_Equal] = {.name = "Symbol::Star_Equal", .length = 2},
    [Token_Kind__Symbol_Tilde] = {.name = "Symbol::Tilde", .length = 1},

    [Token_Kind__Macro_Define] = {.name = "Macro::Define", .length = 6},
    [Token_Kind__Macro_Defined] = {.name = "Macro::Defined", .length = 7},
    [Token_Kind__Macro_Elif] = {.name = "Macro::Elif", .length = 4},
    [Token_Kind__Macro_Elifdef] = {.name = "Macro::Elifdef", .length = 7},
    [Token_Kind__Macro_Elifndef] = {.name = "Macro::Elifndef", .length = 8},
    [Token_Kind__Macro_Else] = {.name = "Macro::Else", .length = 4},
    [Token_Kind__Macro_Embed] = {.name = "Macro::Embed", .length = 5},
    [Token_Kind__Macro_Endif] = {.name = "Macro::Endif", .length = 5},
    [Token_Kind__Macro_Error] = {.name = "Macro::Error", .length = 5},
    [Token_Kind__Macro_Hash] = {.name = "Macro::Hash", .length = 1},
    [Token_Kind__Macro_Hash_Hash] = {.name = "Macro::Hash_Hash", .length = 2},
    [Token_Kind__Macro_Has_C_Attribute] = {.name = "Macro::Has_C_Attribute", .length = 7},
    [Token_Kind__Macro_Has_Embed] = {.name = "Macro::Has_Embed", .length = 5},
    [Token_Kind__Macro_Has_Include] = {.name = "Macro::Has_Include", .length = 7},
    [Token_Kind__Macro_If] = {.name = "Macro::If", .length = 2},
    [Token_Kind__Macro_Ifdef] = {.name = "Macro::Ifdef", .length = 5},
    [Token_Kind__Macro_Ifndef] = {.name = "Macro::Ifndef", .length = 6},
    [Token_Kind__Macro_Include] = {.name = "Macro::Include", .length = 7},
    [Token_Kind__Macro_Line] = {.name = "Macro::Line", .length = 4},
    [Token_Kind__Macro_Pragma] = {.name = "Macro::Pragma", .length = 6},
    [Token_Kind__Macro_Undef] = {.name = "Macro::Undef", .length = 5},
    [Token_Kind__Macro_Warning] = {.name = "Macro::Warning", .length = 7},
};

const char *token_kind_name(Token_Kind kind) {
    return token_kind_table[kind].name;
}

size_t token_kind_length(Token_Kind kind) {
    return token_kind_table[kind].length;
}

void token_stringify(const Token *token, char *buffer, size_t length) {
    size_t printed = 0;
    printed += snprintf(buffer, length - printed, "%s", token_kind_name(token->kind));

    switch (token->kind) {
    case Token_Kind__Identifier:
        printed += snprintf(buffer + printed, length - printed, "(%s)", token->value.as_identifier);
        break;
    case Token_Kind__Value_Char:
        printed += snprintf(buffer + printed, length - printed, "(%c)", token->value.as_char);
        break;
    case Token_Kind__Value_Float64:
        printed += snprintf(buffer + printed, length - printed, "(%f)", token->value.as_float64);
        break;
    case Token_Kind__Value_Int64:
        printed += snprintf(buffer + printed, length - printed, "(%ld)", token->value.as_int64);
        break;
    case Token_Kind__Value_String:
        printed += snprintf(buffer + printed, length - printed, "(%s)", token->value.as_string);
        break;
    case Token_Kind__Value_Uint64:
        printed += snprintf(buffer + printed, length - printed, "(%lu)", token->value.as_uint64);
        break;
    default:
        break;
    }

    buffer[length] = '\0';
}

int token_array_create(Token_Array *array, size_t initial_capacity) {
    Token *items = (Token *)realloc(array->items, initial_capacity * sizeof *items);
    if (items == NULL) {
        return -1;
    }

    array->capacity = initial_capacity;
    array->count = 0;
    array->items = items;

    return 0;
}

void token_array_destroy(Token_Array *array) {
    if (array->items != NULL) {
        free(array->items);
        array->items = NULL;
    }

    array->capacity = 0;
    array->count = 0;
}

static int token_array_grow(Token_Array *array) {
    size_t new_capacity = array->capacity == 0 ? 256 : array->capacity * 2;

    Token *items = (Token *)realloc(array->items, new_capacity * sizeof *items);
    if (items == NULL) {
        return -1;
    }

    array->capacity = new_capacity;
    array->items = items;

    return 0;
}

Token *token_array_push(Token_Array *array) {
    if (array->count >= array->capacity) {
        if (token_array_grow(array) != 0) {
            return NULL;
        }
    }

    return &array->items[array->count++];
}
