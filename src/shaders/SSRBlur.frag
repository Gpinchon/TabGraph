R""(
uniform sampler2D	in_Texture_Color;
uniform vec2		in_Direction;

void ApplyTechnique()
{
	Out.Color = vec4(0);
	vec2 offset1 = in_Direction / 1024.f * (Frag.Material.Roughness + 0.01);
	vec2 offset2 = (in_Direction * 2.f) / 1024.f * (Frag.Material.Roughness + 0.01);
	vec4 sample0 = texture(in_Texture_Color, frag_UV);
	vec4 sample1 = texture(in_Texture_Color, frag_UV + offset1);
	vec4 sample2 = texture(in_Texture_Color, frag_UV - offset1);
	vec4 sample3 = texture(in_Texture_Color, frag_UV + offset2);
	vec4 sample4 = texture(in_Texture_Color, frag_UV - offset2);
	float totalWeight;
	totalWeight += sample0.a;
	totalWeight += sample1.a;
	totalWeight += sample2.a;
	totalWeight += sample3.a;
	totalWeight += sample4.a;
	Out.Color += sample0 * sample0.a;
	Out.Color += sample1 * sample1.a;
	Out.Color += sample2 * sample2.a;
	Out.Color += sample3 * sample3.a;
	Out.Color += sample4 * sample4.a;
	Out.Color /= totalWeight;
}

)""