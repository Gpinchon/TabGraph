R""(
#define	KERNEL_SIZE 9

const vec2 poissonDisk[] = vec2[KERNEL_SIZE](
	vec2(0.95581, -0.18159), vec2(0.50147, -0.35807), vec2(0.69607, 0.35559),
	vec2(-0.0036825, -0.59150),	vec2(0.15930, 0.089750), vec2(-0.65031, 0.058189),
	vec2(0.11915, 0.78449),	vec2(-0.34296, 0.51575), vec2(-0.60380, -0.41527));

void	ApplyTechnique()
{
	if (Frag.Depth == 1)
		return ;
	float	sampleOffset = 0.5f * (1 - Frag.Depth);
	float	sampleAngle = randomAngle(Frag.Position);
	float	s = sin(sampleAngle);
	float	c = cos(sampleAngle);
	vec2	sampleRotation = vec2(c, -s);
	float	occlusion = 0.f;
	for (int i = 0; i < KERNEL_SIZE; i++)
	{
		vec2	sampleUV = Frag.UV + poissonDisk[i] * sampleRotation * sampleOffset;
		vec3	samplePosition = WorldPosition(sampleUV);
		vec3	V = samplePosition - Frag.Position.xyz;
		float	D = length(V);
		float	bias = D + 0.025;
		float	factor = max(0, dot(Frag.Normal, normalize(V)));
		float	angle = max(0, factor - bias);
		occlusion += (angle * (1.f / (1.f + D)));
	}
	occlusion /= float(KERNEL_SIZE);
	Frag.Material.AO += max(0, occlusion);
}

)""