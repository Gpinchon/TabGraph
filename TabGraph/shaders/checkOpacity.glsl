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

//ivec4[4] stipple = ivec4[4](
//	ivec4(10,  0, 15,  5),
//	ivec4(12,  6,  1, 11),
//	ivec4( 2,  8,  7, 13),
//	ivec4( 4, 14,  9,  3)
//);
//
//bool StipplePattern(in vec2 coord) {
//	int noise = int(InterleavedGradientNoise(coord, FrameNumber % 8) * 16);
//	int Offset = int(Depth() * 15);
//	int Range = int(Opacity() * 15);
//	int Max = clamp(Offset + Range, 0, 15);
//	int Min = Max - Range;
//	int X = int(coord.x + noise) % 4;
//	int Y = int(coord.y + noise) % 4;
//	return	stipple[X][Y] >= Min &&
//			stipple[X][Y] <= Max;
//}

bool StipplePattern(in vec2 coord) {
	float noise = InterleavedGradientNoise(coord, FrameNumber % 8);
	return Opacity() > noise;
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
	if (Opacity() < 0.25 || (Opacity() < 0.95f && !StipplePattern(ivec2(gl_FragCoord.xy))))
		discard;
}
)""