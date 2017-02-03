#include <cherry/math/math.h>

union mat4 mat4_identity = (union mat4){
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0 ,0 ,0, 1
};

union mat3 mat3_identity = (union mat3) {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
};

union vec4 quat_identity = (union vec4) {
        0, 0, 0, 1
};
