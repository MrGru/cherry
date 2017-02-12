#version 100

#ifdef GL_ES
        #define input   attribute
        #define output  varying
#else
        #define input   in
        #define output  out
#endif

/*
 * position and texcoord are static data shared among instances
 * z used to sort instances by depth test function after fragment step
 * color is passed to fragment shader
 */
input vec4      position;
input float     z;
input mat4      transform;
input vec4      color;
input float     texid;
input vec2      texcoord_1;
input vec2      texcoord_2;
input vec2      texcoord_3;
input vec2      texcoord_4;
input vec2      texcoord_5;
input vec2      texcoord_6;

/*
 * combine texcoord_i into array to access from vertex id in position.w
 */
vec2            texcoords[6];

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
        texcoords[0] = texcoord_1;
        texcoords[1] = texcoord_2;
        texcoords[2] = texcoord_3;
        texcoords[3] = texcoord_4;
        texcoords[4] = texcoord_5;
        texcoords[5] = texcoord_6;
        gl_Position     = project * view * transform * vec4(position.xyz, 1.0);
        gl_Position.z   = z;
        pixel_color     = color;
        pixel_coord     = texcoords[int(position.w)];
        pixel_texid     = texid;
}
