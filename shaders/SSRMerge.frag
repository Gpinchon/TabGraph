R""(
uniform sampler2D	in_Texture_Color;
uniform sampler2D	in_Last_Texture_Color;
uniform mat4		LastViewMatrix;
uniform mat4		LastProjectionMatrix;

vec4	LastWorldToView(in vec3 position)
{
	return LastViewMatrix * vec4(position, 1);
}

vec4	LastViewToClip(in vec4 position)
{
	return LastProjectionMatrix * position;
}

/** Returns the World position and the scene depth in world units */
vec4	LastWorldToScreen(in vec3 position)
{
	return ClipToScreen(LastViewToClip(LastWorldToView(position)));
}

vec3	GetLastUVz(in vec3 UVz)
{
	vec3 worldPos = ScreenToWorld(UVz.xy, UVz.z);
	UVz = LastWorldToScreen(worldPos).xyz;
	return UVz;
}

vec3	GetLastUVz(in vec2 UV, in float z)
{
	return GetLastUVz(vec3(UV, z));
}

void ApplyTechnique()
{
	vec4 color = texture2D(in_Texture_Color, frag_UV);
	vec4 lastColor = texture2D(in_Last_Texture_Color, GetLastUVz(frag_UV, Frag.Depth).xy);
	float totalWeigh = 0;
	Out.Color = vec4(0);
	totalWeigh += color.a;
	totalWeigh += lastColor.a;
	Out.Color += color * color.a;
	Out.Color += lastColor * lastColor.a;
	//avoid dividing by zero
	Out.Color /= max(totalWeigh, 0.0001);
	Out.Color *= Frag.Depth < 1 ? 1 : 0;
}

)""