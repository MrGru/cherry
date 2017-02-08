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
        struct array *a = array_alloc(sizeof(void *), ORDERED);
        int *p = NULL;
        int *i = smalloc(sizeof(int));
        array_push(a, &p);
        array_push(a, &i);
        array_deep_free_safe(a, int *, sfree);
        cache_free();
        dim_memory();
        return 0;
}
