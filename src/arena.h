#ifndef LEANC_ARENA_H
#define LEANC_ARENA_H

#include <stddef.h>
#include <stdint.h>

typedef struct Arena {
    struct Arena *next;
    size_t capacity;
    size_t count;
    uint8_t data[];
} Arena;

Arena *arena_create(size_t size);
void arena_destroy(Arena *arena);
void *arena_allocate(Arena *arena, size_t size, uint8_t alignment);
char *arena_duplicate_string(Arena *arena, const char *from, size_t length);

#endif // LEANC_ARENA_H
