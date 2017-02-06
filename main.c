#include <cherry/stdio.h>
#include <cherry/memory.h>
#include <cherry/string.h>
#include <cherry/math/math.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/list.h>
#include <cherry/graphic/types.h>
#include <cherry/graphic/image.h>
#include <cherry/graphic/texture.h>

int main(int argc, char const *argv[])
{
        struct string *s = string_alloc_chars("Hello World",sizeof("Hello World")-1);
        debug("%s\n",s->ptr);
        string_free(s);
        cache_free();
        dim_memory();
        return 0;
}
