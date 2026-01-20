// ============================================================================
// File handling.
// ============================================================================

#ifndef __FILE_H__
#define __FILE_H__

int file_get_size(const char *filename);
void *file_load_with_alloc(const char *filename);
int file_load_at_address(const char *filename, void *load_to);
void file_save(const char *filename, void *address, int size);

#endif // __FILE_H__
