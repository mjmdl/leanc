#include "arena.c"
#include "token.c"
#include "lexer.c"

#include <stdio.h>
#include <string.h>

static const char *hello_world_c =
    "include <stdio.h>\n"
    "\n"
    "int main(void) {\n"
    "    printf(\"Hello, World!\n\");\n"
    "    return 0;\n"
    "}\n";

int main(void) {
    Lexer_Status status;
    
    Lexer lexer = {0};
    status = lexer_create(&lexer, hello_world_c, strlen(hello_world_c));
    if (status != Lexer_Status__Ok) {
        fprintf(stderr, "Failed to create the lexer: %s\n", lexer_status_describe(status));
        return -1;
    }

    status = lexer_tokenize(&lexer);
    if (status != Lexer_Status__Ok) {
        fprintf(stderr, "Failed to tokenize input: %s\n", lexer_status_describe(status));
        lexer_destroy(&lexer);
        return -1;
    }

    printf("Tokens tokenized: x%zu\n", lexer.token_array.count);
    for (size_t i = 0; i < lexer.token_array.count; ++i) {
        char buffer[1024];
        token_stringify(&lexer.token_array.items[i], buffer, sizeof buffer);
        printf("Token #%zu: %s\n", i, buffer);
    }

    lexer_destroy(&lexer);
    return 0;
}
