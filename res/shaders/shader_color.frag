#version 100

#ifdef GL_ES
        precision highp float;
        #define input           varying
        #define get_pixel       texture2D
        #define out_pixel       gl_FragColor
#else
        #define input           in
        #define get_pixel       texture
        out vec4                fragColor;
        #define out_pixel       fragColor
#endif

input vec4 pixel_color;

void main()
{
        out_pixel = pixel_color;
}
