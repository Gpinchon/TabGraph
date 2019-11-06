R""(
uniform sampler2D	in_Texture_Color;
uniform sampler2D	in_Last_Texture_Color;
uniform vec2		in_Direction;

void ApplyTechnique()
{
	vec4 color = texture2D(in_Texture_Color, frag_UV);
	vec4 lastColor = texture2D(in_Last_Texture_Color, frag_UV);
	Out.Color = lastColor.a > 0 ? mix(color, lastColor, 0.5) : color;
}

)""