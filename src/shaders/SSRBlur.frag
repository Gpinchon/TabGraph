R""(
uniform sampler2D	in_Texture_Color;
uniform vec2		in_Direction;

void ApplyTechnique()
{
	Out.Color = vec4(0);
	vec2 offset1 = in_Direction / 1024.f * (Frag.Material.Roughness + 0.01);
	vec2 offset2 = (in_Direction * 2.f) / 1024.f * (Frag.Material.Roughness + 0.01);
	const vec4 samples[] = vec4[5](
		texture(in_Texture_Color, frag_UV),
		texture(in_Texture_Color, frag_UV + offset1),
		texture(in_Texture_Color, frag_UV - offset1),
		texture(in_Texture_Color, frag_UV + offset2),
		texture(in_Texture_Color, frag_UV - offset2));
	const float gaussianKernel[] = float[5](0.250301f, 0.221461f, 0.221461f, 0.153388f, 0.153388f);
	for (int i = 0; i < 5; i++) {
		if (samples[i].a > 0)
			Out.Color += samples[i] * gaussianKernel[i];
	}
}

)""