#version 330
precision highp float;
precision highp int;


#define MODE_DXT5       0
#define MODE_YCOCG      1
#define MODE_YCOCGS     2
#define MODE_COCGXY     3


uniform sampler2D image;
uniform int mode;
uniform bool to_srgb;

out vec4 color;

void main()
{
    const float offset = 128.0 / 255.0;

    vec4 v = texelFetch(image, ivec2(gl_FragCoord.xy), 0);

    if(mode == MODE_YCOCGS) {
        float Y = v.a;
        float scale = 1.0 / ((255.0 / 8.0) * v.b + 1);
        float Co = (v.r - offset) * scale;
        float Cg = (v.g - offset) * scale;

        float R = Y + Co - Cg;
        float G = Y + Cg;
        float B = Y - Co - Cg;

        color = vec4(R, G, B, 1);
    }
    else if(mode == MODE_YCOCG) {
        float Y = v.a;
        float Co = v.r - offset;
        float Cg = v.g - offset;

        float R = Y + Co - Cg;
        float G = Y + Cg;
        float B = Y - Co - Cg;

        color = vec4(R, G, B, v.b);
    }
    else if(mode == MODE_COCGXY) {
        float Y = v.a;
        float Co = v.r;
        float Cg = v.g;
        float X = v.b;

        vec3 rgb = 2*Y*vec3(2*Co - Cg, Cg, 2 - 2*Co - Cg);
        color = vec4(rgb, X);
    }
    else
        color = v;

    if(to_srgb)
        color = pow(color, vec4(1/2.2));
}
