R""(
struct RenderHistory {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    sampler2D color;
	sampler2D emissive;
	sampler2D velocity;
};

uniform RenderHistory[8] in_renderHistory;
/*uniform sampler2D	in_Last_Texture_Color;
uniform sampler2D	in_Last_Texture_Emissive;
uniform mat4		LastViewMatrix;
uniform mat4		LastProjectionMatrix;*/

vec4	LastWorldToView(in vec3 position, in int index)
{
	return in_renderHistory[index].viewMatrix * vec4(position, 1);
}

vec4	LastViewToClip(in vec4 position, in int index)
{
	return in_renderHistory[index].projectionMatrix * position;
}

/** Returns the World position and the scene depth in world units */
vec4	LastWorldToScreen(in vec3 position, in int index)
{
	return ClipToScreen(LastViewToClip(LastWorldToView(position, index), index));
}

vec3	GetLastUVz(in vec3 UVz, in int index)
{
	vec3 worldPos = ScreenToWorld(UVz.xy, UVz.z);
	UVz = LastWorldToScreen(worldPos, index).xyz;
	return UVz;
}

vec3	GetLastUVz(in vec2 UV, in float z, in int index)
{
	return GetLastUVz(vec3(UV, z), index);
}


/*vec4[4] GatherTexture(sampler2D tex, vec2 uv) {
	vec4 x = textureGather(tex, uv, 0);
	vec4 y = textureGather(tex, uv, 1);
	vec4 z = textureGather(tex, uv, 2);
	vec4 w = textureGather(tex, uv, 3);
 	return vec4[4](
		vec4(x[0], y[0], z[0], w[0]),
		vec4(x[1], y[1], z[1], w[1]),
		vec4(x[2], y[2], z[2], w[2]),
		vec4(x[3], y[3], z[3], w[3])
	);
}*/

vec4[5] GatherTexture(sampler2D tex, vec2 uv) {
 	return vec4[5](
		textureOffset(tex, uv, ivec2(0, 0)),
		textureOffset(tex, uv, ivec2(1, 1)),
		textureOffset(tex, uv, ivec2(-1, 1)),
		textureOffset(tex, uv, ivec2(1, -1)),
		textureOffset(tex, uv, ivec2(-1, -1))
		//texelFetchOffset(tex, uv, 0, ivec2(-1, 0)),
		//texelFetchOffset(tex, uv, 0, ivec2(0, -1)),
		//texelFetchOffset(tex, uv, 0, ivec2(-1, -1)),
		//texelFetchOffset(tex, uv, 0, ivec2(1, -1)),
		//texelFetchOffset(tex, uv, 0, ivec2(-1, 1))
	);
}

uint[9] GatherIDTexture(usampler2D tex, ivec2 uv) {
	uvec4 i = textureGather(tex, uv, 0);
 	return uint[9](
		texelFetchOffset(tex, uv, 0, ivec2(0, 0)),
		texelFetchOffset(tex, uv, 0, ivec2(1, 0)),
		texelFetchOffset(tex, uv, 0, ivec2(0, 1)),
		texelFetchOffset(tex, uv, 0, ivec2(1, 1)),
		texelFetchOffset(tex, uv, 0, ivec2(-1, 0)),
		texelFetchOffset(tex, uv, 0, ivec2(0, -1)),
		texelFetchOffset(tex, uv, 0, ivec2(-1, -1)),
		texelFetchOffset(tex, uv, 0, ivec2(1, -1)),
		texelFetchOffset(tex, uv, 0, ivec2(-1, 1))
	);
}

float InterleavedGradientNoise(vec2 uv, float FrameId)
{
	// magic values are found by experimentation
	uv += FrameId * (vec2(47, 17) * 0.695f);

    const vec3 magic = vec3( 0.06711056f, 0.00583715f, 52.9829189f );
    return fract(magic.z * fract(dot(uv, magic.xy)));
}

void TemporalAccumulation()
{
	out_0 = vec4(0);
	out_1 = vec4(0);
	float samplesWeight = 0;
	int samplesNbr = 0;
	for(int i = 0; i < 8; ++i)
	{
		vec2 lastCoord = GetLastUVz(ScreenTexCoord(), Depth(), i).xy;
		//float noise = InterleavedGradientNoise(gl_FragCoord.xy, i) * 2 - 1;
		//lastCoord += noise * 0.001;
		vec4 sample_0 = texture(in_renderHistory[i].color, lastCoord, 0);
		vec4 sample_1 = texture(in_renderHistory[i].emissive, lastCoord, 0);
		float velocity = texture(in_renderHistory[i].velocity, lastCoord, 0).r;
		if (velocity > 0.001 || lastCoord.x < 0 || lastCoord.x > 1 || lastCoord.y < 0 || lastCoord.y > 1)
			continue;
		//float weight = 1 - i / 8.f;
		float weight = 9.f / float(i + 1);
		out_0 += sample_0 * weight;
		out_1 += sample_1 * weight;
		samplesWeight += weight;
		++samplesNbr;
	}
	out_0 /= max(1, samplesWeight);
	out_1 /= max(1, samplesWeight);
	out_0.a = samplesNbr / 9.f;
	out_1.a = samplesNbr / 9.f;
	if (Depth() == 1) {
		out_0 = vec4(0, 0, 0, 0);
		out_1 = vec4(0, 0, 0, 0);
	}
}

)""
