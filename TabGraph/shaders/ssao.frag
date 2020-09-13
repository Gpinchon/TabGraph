R""(
#if SSAO_QUALITY == 1
#define	KERNEL_SIZE 6
#define SAMPLES 3
#elif SSAO_QUALITY == 2
#define	KERNEL_SIZE 10
#define SAMPLES 3
#elif SSAO_QUALITY == 3
#define	KERNEL_SIZE 10
#define SAMPLES 6
#else //SSAO_QUALITY == 4
#define	KERNEL_SIZE 16
#define SAMPLES 8
#endif

#define STRENGTH 1.f
#define RADIUS 0.05f

void	ApplyTechnique()
{
	float	sampleAngle = random(Frag.CubeUV);
	/*float	s = sin(sampleAngle);
	float	c = cos(sampleAngle);
	vec2	sampleRotation = vec2(c, -s);*/
	float	occlusion = 0.f;
	float	circleSteps = 1 / float(KERNEL_SIZE) * 6.283285; //2 * M_PI
	float	circleAngle = sampleAngle * circleSteps;
	for (int i = 0; i < KERNEL_SIZE; ++i)
	{
		circleAngle += circleSteps;
		//float	progress = i / float(KERNEL_SIZE);
		//float angle = progress * 2 * M_PI;
		//angle += sampleAngle;
		//circleAngle = mod(circleAngle, 2 * M_PI);
		vec2	endPoint = vec2(cos(circleAngle), sin(circleAngle));
		//endPoint *= sampleRotation;
		endPoint *= RADIUS;
		endPoint += Frag.UV;
		float	samplingOcclusion = 0;
		for (int sampleNum = 1; sampleNum <= SAMPLES; ++sampleNum)
		{
			vec2	screenCoords = mix(Frag.UV, endPoint, sampleNum / float(SAMPLES));
			vec3	diff = WorldPosition(screenCoords) - Frag.Position; 
			const vec3 v = normalize(diff); 
			const float d = length(diff) * RADIUS;
			samplingOcclusion += max(0.0, dot(Frag.Normal, v) - 0.025) * (1.0 / (1.0 + d)) * STRENGTH;
		}
		occlusion += samplingOcclusion / SAMPLES;
	}
	occlusion /= float(KERNEL_SIZE);
	//Out.Color.rgb = vec3(sampleAngle);
	Out.Color.rgb = vec3(Frag.AO + max(0, occlusion));
	Out.Color.a = 1;
	Out.Emitting = vec3(0);
}

)""