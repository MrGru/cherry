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

output vec4     pixel_color;

uniform mat4    project;
uniform mat4    view;

void main()
{
        gl_Position = project * view * transform * vec4(position, 1.0);
        gl_Position.z = z;
        pixel_color = color;
}
