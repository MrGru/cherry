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
 * texroot and texrange used to detect area in image will be used to draw
 *      A----------------------------------B
 *      ------------R+++++++++w-------------
 *      ------------+         +-------------
 *      ------------+         +-------------
 *      ------------h++++++++++-------------
 *      D----------------------------------C
 *      base texcoord is all image define by rectangle ABCD, A is root
 *      we can easily transform point p in ABCD to Rwh space :
 *              x = R.x + w * p.x;
 *              y = R.y + h * p.y;
 */
input vec3      position;
input float     z;
input mat4      transform;
input vec4      color;
input vec2      texcoord;
input vec2      texroot;
input vec2      texrange;
input float     texid;

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
        gl_Position     = project * view * transform * vec4(position, 1.0);
        gl_Position.z   = z;
        pixel_color     = color;
        pixel_coord     = texroot + texrange * texcoord;
        pixel_texid     = texid;
}
