R""(
ivec4 stipple = ivec4(
    1, 0,
    0, 0
);

int StipplePattern(in vec2 coord) {
	uint offset = DrawID % 4;
	//uint offset = int(gl_FragCoord.z * 4) % 4;
	uint index = uint((mod(coord.x, 2) + mod(coord.y, 2) * 2) + offset) % 4;
	return stipple[index];
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
	uint offset = DrawID % 4;
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
	if (Opacity() < 0.003 || StipplePattern(gl_FragCoord.xy - 0.5f) == 0)
		discard;
}
)""