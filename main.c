#include <stdio.h>
#include <cherry/memory.h>
#include <cherry/string.h>
#include <cherry/math/math.h>

int main(int argc, char const *argv[])
{
        char *s = string_alloc(0);
        s = string_cat(s, "Hello World", sizeof("Hello World")-1);
        printf("%ld\n", string_len(s));
        string_free(s);
        dim_memory();
        return 0;
}
