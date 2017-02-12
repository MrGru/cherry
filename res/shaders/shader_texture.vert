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
input float     texid;
input vec4      color;
input mat4      transform;
input vec4      texcoord_1;
input vec4      texcoord_2;
input vec4      texcoord_3;
input vec4      vertex_1;
input vec4      vertex_2;
input vec4      vertex_3;

/*
 * combine texcoord_i, vertex_i into array to access from vid
 */
vec2            texcoords[6];
vec2            vertice[6];

/*
 * pixel_coord used to fetch next pixel in image applied to fragment
 * pixel_texid used to detech which image will be used
 * pixel_color used to adjust fragment color
 */
output vec4     pixel_color;
output vec2     pixel_coord;
output float    pixel_texid;

uniform mat4    project;
uniform mat4    view;

void main()
{
        /*
         * group texcoords
         */
        texcoords[0]    = texcoord_1.xy;
        texcoords[1]    = texcoord_1.zw;
        texcoords[2]    = texcoord_2.xy;
        texcoords[3]    = texcoord_2.zw;
        texcoords[4]    = texcoord_3.xy;
        texcoords[5]    = texcoord_3.zw;

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
        pixel_coord     = texcoords[int(vid)];
        pixel_texid     = texid;
}
