#include <cherry/math/math.h>

union mat4 mat4_identity = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
};

union vec4 quat_identity = {
        0, 0, 0, 1
};
