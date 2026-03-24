// ============================================================================
// PC implementation of lib/file.h via standard C stdio.
// ============================================================================

#include "lib/file.h"
#include <stdio.h>
#include <stdlib.h>

int file_get_size(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END);
    int size = (int)ftell(f);
    fclose(f);
    return size;
}

void *file_load_with_alloc(const char *filename)
{
    int size = file_get_size(filename);
    if (!size) return NULL;
    void *data = malloc((size_t)size);
    if (!data) return NULL;
    if (!file_load_at_address(filename, data)) {
        free(data);
        return NULL;
    }
    return data;
}

int file_load_at_address(const char *filename, void *load_to)
{
    int size = file_get_size(filename);
    if (!size) return 0;
    FILE *f = fopen(filename, "rb");
    if (!f) return 0;
    size_t n = fread(load_to, 1, (size_t)size, f);
    fclose(f);
    return (n == (size_t)size) ? 1 : 0;
}

void file_save(const char *filename, void *address, int size)
{
    FILE *f = fopen(filename, "wb");
    if (!f) return;
    fwrite(address, 1, (size_t)size, f);
    fclose(f);
}
