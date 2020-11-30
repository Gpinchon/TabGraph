R""(
ivec4 stipple = ivec4(
    1, 0,
    0, 0
);

int StipplePattern(in vec2 coord) {
	uint offset = DrawID % 4;
	uint index = uint((mod(coord.x, 2) + mod(coord.y, 2) * 2) + offset) % 4;
	return stipple[index];
}

void	CheckOpacity()
{
	if (Opacity() >= 0.99f) {
		SetOpacity(1);
		return;
	}
	if (StandardValues.OpacityMode == OPAQUE) {
		SetOpacity(1);
		return;
	}
	if (StandardValues.OpacityMode == MASK) {
		if(Opacity() > StandardValues.OpacityCutoff) {
			SetOpacity(1);
			return;
		}
		discard;
	}
	if (Opacity() < 0.003 || StipplePattern(gl_FragCoord.xy - 0.5f) == 0)
		discard;
}
)""