#version 100

#ifdef GL_ES
        #define input   attribute
        #define output  varying
#else
        #define input   in
        #define output  out
#endif

/*
 * z used to sort instances by depth test function after fragment step
 * vid used to access current texcoord, vertex
 */
input float     vid;
input float     z;
input mat4      transform;
input vec4      color;
input vec4      vertex_1;
input vec4      vertex_2;
input vec4      vertex_3;

/*
 * combine vertex_i into array to access from vid
 */
vec2            vertice[6];

/*
 * pixel_color used to adjust fragment color
 */
output vec4     pixel_color;

uniform mat4    project;
uniform mat4    view;

void main()
{
        /*
         * group vertice
         */
        vertice[0]      = vertex_1.xy;
        vertice[1]      = vertex_1.zw;
        vertice[2]      = vertex_2.xy;
        vertice[3]      = vertex_2.zw;
        vertice[4]      = vertex_3.xy;
        vertice[5]      = vertex_3.zw;

        /*
         * working for 2d rendering so pos.z = 0
         */
        vec2 pos        = vertice[int(vid)];
        gl_Position     = project * view * transform * vec4(vec3(pos, 0.0), 1.0);
        /*
         * set position z for depth test working
         */
        gl_Position.z   = z;
        /*
         * pass pixel parameters
         */
        pixel_color     = color;
}
