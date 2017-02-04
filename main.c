#include <cherry/stdio.h>
#include <cherry/memory.h>
#include <cherry/string.h>
#include <cherry/math/math.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/graphic/types.h>

int main(int argc, char const *argv[])
{
        struct array *p = array_alloc(sizeof(int), UNORDERED);
        array_push(p, &(int){0});
        array_push(p, &(int){1});
        array_push(p, &(int){2});
        int *n;
        int i;
        array_for_each_index(n, i, p) {
                printf("%d\n", *n);
                array_remove(p, i);
                i--;
                n--;
        }
        array_free(p);
        dim_memory();
        return 0;
}
