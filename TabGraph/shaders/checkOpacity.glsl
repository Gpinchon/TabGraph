R""(

uvec3 Rand3DPCG16(ivec3 p)
{
	uvec3 v = uvec3(p);
	v = v * 1664525u + 1013904223u;
	v.x += v.y*v.z;
	v.y += v.z*v.x;
	v.z += v.x*v.y;
	v.x += v.y*v.z;
	v.y += v.z*v.x;
	v.z += v.x*v.y;
	return v >> 16u;
}

float InterleavedGradientNoise(vec2 uv, float FrameId)
{
	// magic values are found by experimentation
	uv += FrameId * (vec2(47, 17) * 0.695f);

    const vec3 magic = vec3( 0.06711056f, 0.00583715f, 52.9829189f );
    return fract(magic.z * fract(dot(uv, magic.xy)));
}


mat4 thresholdMatrix = mat4(
1.0 / 17.0,  9.0 / 17.0,  3.0 / 17.0, 11.0 / 17.0,
13.0 / 17.0,  5.0 / 17.0, 15.0 / 17.0,  7.0 / 17.0,
4.0 / 17.0, 12.0 / 17.0,  2.0 / 17.0, 10.0 / 17.0,
16.0 / 17.0,  8.0 / 17.0, 14.0 / 17.0,  6.0 / 17.0
);

ivec4 stipple = ivec4(
	1, 0,
	0, 0
);

int StipplePattern(in vec2 coord) {
	uint offset = (FrameNumber + DrawID) % 4;
	//float noise = InterleavedGradientNoise(coord, FrameNumber % 8);
	//return noise > 0.5 ? 1 : 0;
	//uint offset = (DrawID + int(noise * 117)) % 4;
	uint x = int(coord.x) % 2;
	uint y = int(coord.y) % 2;
	uint index = x + y * 2 + offset;
	return stipple[index % 4];
}

float StipplePattern(float alpha, vec2 vP)
{
	const float jitterTable[4] =
	{
		float( 0.00 ),
		float( 0.26 ),
		float( 0.51 ),
		float( 0.76 )
	};
	float jitNo = 0.0;
	ivec2 vPI = ivec2(0);
	vPI.x = int(mod(vP.x, 2));
	vPI.y = int(mod(vP.y, 2));
	int jitterIndex = vPI.x + 2 * vPI.y;
	//uint offset = 0;
	uint offset = FrameNumber % 4 + DrawID % 4;
	//uint offset = int(gl_FragCoord.z * 4) % 4;
	jitNo = jitterTable[(jitterIndex + offset) % 4];
	if (jitNo > alpha)
		return 0;
	return 1;
}

void	CheckOpacity()
{
	if (Opacity() >= 0.95f) {
		SetOpacity(1);
		return;
	}
	#if OPACITYMODE == OPAQUE
		SetOpacity(1);
		return;
	#endif
	#if OPACITYMODE == MASK
		if(Opacity() > StandardValues.OpacityCutoff) {
			SetOpacity(1);
			return;
		}
		discard;
	#endif
	if (Opacity() < 0.003 || StipplePattern(ivec2(gl_FragCoord.xy)) == 0)
		discard;
}
)""