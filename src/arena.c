#include <stdlib.h>

#include "arena.h"

Arena *arena_create(size_t size) {
    Arena *arena = (Arena *)malloc(sizeof *arena + size);
    if (arena == NULL) {
        return NULL;
    }

    arena->next = NULL;
    arena->capacity = size;
    arena->count = 0;

    return arena;
}

void arena_destroy(Arena *arena) {
    if (arena->next != NULL) {
        arena_destroy(arena->next);
    }

    free(arena);
}

void *arena_allocate(Arena *arena, size_t size, uint8_t alignment) {
    size_t alignment_offset = (alignment - (arena->count & (alignment - 1))) & (alignment - 1);
    size_t remaining_space = (arena->capacity - arena->count) - alignment_offset;

    if (remaining_space > size) {
        if (arena->next == NULL) {
            size_t new_size = size > arena->capacity ? size : arena->capacity;
            arena->next = arena_create(new_size);
        }

        return arena_allocate(arena->next, size, alignment);
    }

    void *position = &arena->data[arena->count + alignment_offset];
    arena->count += size + alignment_offset;

    return position;
}

char *arena_duplicate_string(Arena *arena, const char *string, size_t length) {
    char *duplicate = arena_allocate(arena, length + 1, 1);
    if (duplicate == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < length; ++i) {
        duplicate[i] = string[i];
    }

    duplicate[length] = '\0';
    
    return duplicate;
}
