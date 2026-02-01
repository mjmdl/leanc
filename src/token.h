#ifndef LEANC_TOKEN_H
#define LEANC_TOKEN_H

#include <stddef.h>
#include <stdint.h>

typedef enum Token_Kind {
    Token_Kind__None,
    
    Token_Kind__Identifier,

    Token_Kind__Value_Char,
    Token_Kind__Value_Float64,
    Token_Kind__Value_Int64,
    Token_Kind__Value_String,
    Token_Kind__Value_Uint64,
    
    Token_Kind__Keyword_Alignas,
    Token_Kind__Keyword_Alignof,
    Token_Kind__Keyword_Asm,
    Token_Kind__Keyword_Atomic,
    Token_Kind__Keyword_Auto,
    Token_Kind__Keyword_Bigint,
    Token_Kind__Keyword_Bool,
    Token_Kind__Keyword_Break,
    Token_Kind__Keyword_Case,
    Token_Kind__Keyword_Char,
    Token_Kind__Keyword_Complex,
    Token_Kind__Keyword_Const,
    Token_Kind__Keyword_Constexpr,
    Token_Kind__Keyword_Continue,
    Token_Kind__Keyword_Decimal128,
    Token_Kind__Keyword_Decimal32,
    Token_Kind__Keyword_Decimal64,
    Token_Kind__Keyword_Default,
    Token_Kind__Keyword_Do,
    Token_Kind__Keyword_Double,
    Token_Kind__Keyword_Else,
    Token_Kind__Keyword_Enum,
    Token_Kind__Keyword_Extern,
    Token_Kind__Keyword_False,
    Token_Kind__Keyword_Float,
    Token_Kind__Keyword_For,
    Token_Kind__Keyword_Generic,
    Token_Kind__Keyword_Goto,
    Token_Kind__Keyword_If,
    Token_Kind__Keyword_Imaginary,
    Token_Kind__Keyword_Inline,
    Token_Kind__Keyword_Int,
    Token_Kind__Keyword_Long,
    Token_Kind__Keyword_Noreturn,
    Token_Kind__Keyword_Nullptr,
    Token_Kind__Keyword_Register,
    Token_Kind__Keyword_Restrict,
    Token_Kind__Keyword_Return,
    Token_Kind__Keyword_Short,
    Token_Kind__Keyword_Signed,
    Token_Kind__Keyword_Sizeof,
    Token_Kind__Keyword_Static,
    Token_Kind__Keyword_Static_Assert,
    Token_Kind__Keyword_Struct,
    Token_Kind__Keyword_Switch,
    Token_Kind__Keyword_Thread_Local,
    Token_Kind__Keyword_True,
    Token_Kind__Keyword_Typedef,
    Token_Kind__Keyword_Typeof,
    Token_Kind__Keyword_Typeof_Unqual,
    Token_Kind__Keyword_Union,
    Token_Kind__Keyword_Unsigned,
    Token_Kind__Keyword_Void,
    Token_Kind__Keyword_Volatile,
    Token_Kind__Keyword_While,
    
    Token_Kind__Symbol_And,
    Token_Kind__Symbol_And_And,
    Token_Kind__Symbol_And_Equal,
    Token_Kind__Symbol_Arrow,
    Token_Kind__Symbol_Caret,
    Token_Kind__Symbol_Caret_Equal,
    Token_Kind__Symbol_Colon,
    Token_Kind__Symbol_Comma,
    Token_Kind__Symbol_Curly_Left,
    Token_Kind__Symbol_Curly_Right,
    Token_Kind__Symbol_Dot,
    Token_Kind__Symbol_Ellipsis,
    Token_Kind__Symbol_Equal,
    Token_Kind__Symbol_Equal_Equal,
    Token_Kind__Symbol_Greater,
    Token_Kind__Symbol_Greater_Equal,
    Token_Kind__Symbol_Less,
    Token_Kind__Symbol_Less_Equal,
    Token_Kind__Symbol_Minus,
    Token_Kind__Symbol_Minus_Equal,
    Token_Kind__Symbol_Minus_Minus,
    Token_Kind__Symbol_Modulo,
    Token_Kind__Symbol_Modulo_Equal,
    Token_Kind__Symbol_Not,
    Token_Kind__Symbol_Not_Equal,
    Token_Kind__Symbol_Or,
    Token_Kind__Symbol_Or_Equal,
    Token_Kind__Symbol_Or_Or,
    Token_Kind__Symbol_Paren_Left,
    Token_Kind__Symbol_Paren_Right,
    Token_Kind__Symbol_Plus,
    Token_Kind__Symbol_Plus_Equal,
    Token_Kind__Symbol_Plus_Plus,
    Token_Kind__Symbol_Question,
    Token_Kind__Symbol_Semicolon,
    Token_Kind__Symbol_Shift_Left,
    Token_Kind__Symbol_Shift_Left_Equal,
    Token_Kind__Symbol_Shift_Right,
    Token_Kind__Symbol_Shift_Right_Equal,
    Token_Kind__Symbol_Slash,
    Token_Kind__Symbol_Slash_Equal,
    Token_Kind__Symbol_Square_Left,
    Token_Kind__Symbol_Square_Right,
    Token_Kind__Symbol_Star,
    Token_Kind__Symbol_Star_Equal,
    Token_Kind__Symbol_Tilde,

    Token_Kind__Macro_Define,
    Token_Kind__Macro_Defined,
    Token_Kind__Macro_Elif,
    Token_Kind__Macro_Elifdef,
    Token_Kind__Macro_Elifndef,
    Token_Kind__Macro_Else,
    Token_Kind__Macro_Embed,
    Token_Kind__Macro_Endif,
    Token_Kind__Macro_Error,
    Token_Kind__Macro_Hash,
    Token_Kind__Macro_Hash_Hash,
    Token_Kind__Macro_Has_C_Attribute,
    Token_Kind__Macro_Has_Embed,
    Token_Kind__Macro_Has_Include,
    Token_Kind__Macro_If,
    Token_Kind__Macro_Ifdef,
    Token_Kind__Macro_Ifndef,
    Token_Kind__Macro_Include,
    Token_Kind__Macro_Line,
    Token_Kind__Macro_Pragma,
    Token_Kind__Macro_Undef,
    Token_Kind__Macro_Warning,

    Token_Kind__Enum_Count,
} Token_Kind;

typedef union Token_Value {
    uint64_t as_uint64;
    int64_t as_int64;
    double as_float64;
    char as_char;
    const char *as_string;
    const char *as_identifier;
} Token_Value;

typedef struct Token {
    Token_Kind kind;
    Token_Value value;
    size_t position;
    size_t line;
    size_t column;
    size_t length;
} Token;

typedef struct Token_Array {
    Token *items;
    size_t capacity;
    size_t count;
} Token_Array;

const char *token_kind_name(Token_Kind kind);
size_t token_kind_length(Token_Kind kind);

void token_stringify(const Token *token, char *buffer, size_t length);

int token_array_create(Token_Array *array, size_t initial_capacity);
void token_array_destroy(Token_Array *array);
Token *token_array_push(Token_Array *array);

#endif // LEANC_TOKEN_H
