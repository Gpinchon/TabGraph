R""(
uniform sampler2D	in_Texture_Color;
uniform vec2		in_Direction;

void ApplyTechnique()
{
	Out.Color = vec4(0);
	vec2 offset1 = in_Direction / textureSize(in_Texture_Color, 0);// * max(Frag.Material.Roughness, 0.1);
	vec2 offset2 = (in_Direction * 2.f) / textureSize(in_Texture_Color, 0);// * max(Frag.Material.Roughness, 0.1);
	vec4 sample0 = texture(in_Texture_Color, frag_UV);
	vec4 sample1 = texture(in_Texture_Color, frag_UV + offset1);
	vec4 sample2 = texture(in_Texture_Color, frag_UV - offset1);
	vec4 sample3 = texture(in_Texture_Color, frag_UV + offset2);
	vec4 sample4 = texture(in_Texture_Color, frag_UV - offset2);
	float weight0 = sample0.a;
	float weight1 = sample1.a * (max(0, dot(Frag.Normal, texture(Texture.Normal, frag_UV + offset1).xyz)));
	float weight2 = sample2.a * (max(0, dot(Frag.Normal, texture(Texture.Normal, frag_UV - offset1).xyz)));
	float weight3 = sample3.a * (max(0, dot(Frag.Normal, texture(Texture.Normal, frag_UV + offset2).xyz)));
	float weight4 = sample4.a * (max(0, dot(Frag.Normal, texture(Texture.Normal, frag_UV - offset2).xyz)));	
	float totalWeight;
	totalWeight += weight0;
	totalWeight += weight1;
	totalWeight += weight2;
	totalWeight += weight3;
	totalWeight += weight4;
	Out.Color += sample0 * weight0;
	Out.Color += sample1 * weight1;
	Out.Color += sample2 * weight2;
	Out.Color += sample3 * weight3;
	Out.Color += sample4 * weight4;
	Out.Color /= totalWeight;
	/*
	Out.Color = vec4(0);
	vec2 offset1 = in_Direction / textureSize(in_Texture_Color, 0) * max(Frag.Material.Roughness, 0.1);
	vec2 offset2 = (in_Direction * 2.f) / textureSize(in_Texture_Color, 0) * max(Frag.Material.Roughness, 0.1);
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
	*/
}

)""