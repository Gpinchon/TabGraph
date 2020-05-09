R""(

//layout(location = 0) out vec4	out_Buffer0;
//layout(location = 1) out vec4	out_Buffer1;
//layout(location = 2) out vec4	out_Buffer2;
//layout(location = 3) out vec4	out_Buffer3;
layout(location = 4) out vec4	out_Buffer4;
//layout(location = 5) out vec4	out_Buffer5;

uniform sampler2D   in_Texture_SSAO;

in vec2             frag_UV;

void main()
{
    out_Buffer4.r = texture(in_Texture_SSAO, frag_UV).r;
}

)""