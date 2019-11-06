R""(
uniform sampler2D	in_Texture_Color;
uniform sampler2D	in_Last_Texture_Color;
uniform vec2		in_Direction;

void ApplyTechnique()
{
	Out.Color = vec4(0);
	Out.Color = mix(texture2D(in_Texture_Color, frag_UV), texture2D(in_Last_Texture_Color, frag_UV), 0.5);
}

)""