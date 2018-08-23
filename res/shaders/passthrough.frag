#version 410
layout(location = 0) out vec4	out_Albedo;
layout(location = 1) out vec4	out_Fresnel;
layout(location = 2) out vec4	out_Emitting;
layout(location = 3) out vec4	out_Material_Values;
layout(location = 4) out vec4	out_BRDF;
layout(location = 5) out vec4	out_Normal;
layout(location = 6) out vec4	out_Position;

uniform sampler2D	in_Texture_Albedo;
uniform sampler2D	in_Texture_Fresnel;
uniform sampler2D	in_Texture_Emitting;
uniform sampler2D	in_Texture_Material_Values;
uniform sampler2D	in_Texture_BRDF;
uniform sampler2D	in_Texture_Normal;
uniform sampler2D	in_Texture_Position;
uniform sampler2D	in_Texture_Depth;

in vec2	frag_UV;

void main()
{
	out_Albedo = texture(in_Texture_Albedo, frag_UV);
	out_Fresnel = texture(in_Texture_Fresnel, frag_UV);
	out_Emitting = texture(in_Texture_Emitting, frag_UV);
	out_Material_Values = texture(in_Texture_Material_Values, frag_UV);
	out_BRDF = texture(in_Texture_BRDF, frag_UV);
	out_Normal = texture(in_Texture_Normal, frag_UV);
	out_Position = texture(in_Texture_Position, frag_UV);
}