R""(
uniform sampler2D	in_Texture_Color;
uniform vec2		in_Direction;

void ApplyTechnique()
{
	Out.Color = vec4(0);
	vec2 offset1 = in_Direction / 1024.f * (Frag.Material.Roughness + 0.1);
	vec2 offset2 = (in_Direction * 2.f) / 1024.f * (Frag.Material.Roughness + 0.1);
	Out.Color += texture2D(in_Texture_Color, frag_UV) * 0.250301f;
	Out.Color += texture2D(in_Texture_Color, frag_UV + offset1) * 0.221461f;
	Out.Color += texture2D(in_Texture_Color, frag_UV - offset1) * 0.221461f;
	Out.Color += texture2D(in_Texture_Color, frag_UV + offset2) * 0.153388f;
	Out.Color += texture2D(in_Texture_Color, frag_UV - offset2) * 0.153388f;
}

)""