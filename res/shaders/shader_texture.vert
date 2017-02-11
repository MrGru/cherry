#version 100

#ifdef GL_ES
        #define input   attribute
        #define output  varying
#else
        #define input   in
        #define output  out
#endif

input vec3      position;
input float     z;
input mat4      transform;
input vec4      color;
input vec2      texcoord;
input float     texid;

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
        pixel_coord     = texcoord;
        pixel_texid     = texid;
}
