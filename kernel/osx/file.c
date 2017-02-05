#include <cherry/platform.h>

#if OS == OSX

#include <cherry/stdio.h>
#include <cherry/string.h>

FILE *file_open(char *file, char *mode)
{
        return fopen(file, mode);
}

struct string *file_full_path(char *file)
{
        return string_alloc_chars(file, strlen(file));
}

#endif
