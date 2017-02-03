#include <stdio.h>
#include <cherry/memory.h>
#include <cherry/string.h>
#include <cherry/math/math.h>
#include <cherry/array.h>
#include <cherry/map.h>

int main(int argc, char const *argv[])
{
        struct string *s = string_alloc(10);
        string_cat(s, "Hello World", sizeof("Hello World") - 1);

        struct map *m = map_alloc(sizeof(int));
        map_set(m, qlkey("123"), &s);
        struct string *p = map_get(m, struct string *, qlkey("123"));
        printf("%s\n", p->ptr);
        map_deep_free(m, struct string *, string_free);

        dim_memory();
        return 0;
}
