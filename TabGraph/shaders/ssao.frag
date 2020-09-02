R""(
#define	KERNEL_SIZE 8
#define SAMPLES 4
#define STRENGTH 1.f
#define RADIUS 0.05f

void	ApplyTechnique()
{
	float	sampleAngle = randomAngle(Frag.Position);
	float	s = sin(sampleAngle);
	float	c = cos(sampleAngle);
	vec2	sampleRotation = vec2(c, -s);
	float	occlusion = 0.f;

	for (int i = 0; i < KERNEL_SIZE; ++i)
	{
		float	progress = i / float(KERNEL_SIZE);
		vec2	endPoint = vec2(cos(progress * 2 * M_PI), sin(progress * 2 * M_PI));
		endPoint *= sampleRotation;
		endPoint *= RADIUS;
		endPoint += Frag.UV;
		float	samplingOcclusion = 0;
		for (int sampleNum = 1; sampleNum <= SAMPLES; ++sampleNum)
		{
			vec2 screenCoords = mix(Frag.UV, endPoint, sampleNum / float(SAMPLES));
			vec3	diff = WorldPosition(screenCoords) - Frag.Position; 
			const vec3 v = normalize(diff); 
			const float d = length(diff) * RADIUS;
			samplingOcclusion += max(0.0, dot(Frag.Normal, v) - 0.025) * (1.0 / (1.0 + d)) * STRENGTH;
		}
		occlusion += samplingOcclusion / SAMPLES;
	}
	occlusion /= float(KERNEL_SIZE);
	Out.Color.rgb = vec3(Frag.AO + max(0, occlusion));
	Out.Color.a = 1;
	Out.Emitting = vec3(0);
}

)""