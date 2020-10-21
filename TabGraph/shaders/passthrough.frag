R""(
layout(location = 0) out vec4	out_Buffer0;
layout(location = 1) out vec4	out_Buffer1;
layout(location = 2) out vec4	out_Buffer2;
layout(location = 3) out vec4	out_Buffer3;
layout(location = 4) out vec4	out_Buffer4;
layout(location = 5) out vec4	out_Buffer5;

uniform sampler2D	in_Buffer0;
uniform sampler2D	in_Buffer1;
uniform sampler2D	in_Buffer2;
uniform sampler2D	in_Buffer3;
uniform sampler2D	in_Buffer4;
uniform sampler2D	in_Buffer5;
uniform sampler2D	in_Texture_Depth;

in vec2	frag_UV;

void PassThrough()
{
	out_Buffer0 = texture(in_Buffer0, frag_UV);
	out_Buffer1 = texture(in_Buffer1, frag_UV);
	out_Buffer2 = texture(in_Buffer2, frag_UV);
	out_Buffer3 = texture(in_Buffer3, frag_UV);
	out_Buffer4 = texture(in_Buffer4, frag_UV);
	out_Buffer5 = texture(in_Buffer5, frag_UV);
	gl_FragDepth = texture(in_Texture_Depth, frag_UV).r;
}

)""