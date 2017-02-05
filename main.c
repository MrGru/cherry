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
        struct texture *p = texture_alloc_file("wolf.jpg");
        texture_bind(p);
        debug("%d\n", p->active_id);
        
        cache_free();
        dim_memory();
        return 0;
}
