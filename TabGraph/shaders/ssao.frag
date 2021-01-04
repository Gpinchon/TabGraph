R""(
#if SSAO_QUALITY == 1
#define	KERNEL_SIZE 6
#define SAMPLES 3
#elif SSAO_QUALITY == 2
#define	KERNEL_SIZE 8
#define SAMPLES 4
#elif SSAO_QUALITY == 3
#define	KERNEL_SIZE 4
#define SAMPLES 4
#else //SSAO_QUALITY == 4
#define	KERNEL_SIZE 16
#define SAMPLES 8
#endif

#define STRENGTH 1.f
#define RADIUS 0.05f

float InterleavedGradientNoise(vec2 uv, float FrameId)
{
	// magic values are found by experimentation
	uv += FrameId * (vec2(47, 17) * 0.695f);

    const vec3 magic = vec3( 0.06711056f, 0.00583715f, 52.9829189f );
    return fract(magic.z * fract(dot(uv, magic.xy)));
}

void	SSAO()
{
	//float sampleAngle = InterleavedGradientNoise(ScreenTexCoord(), FrameNumber % 8) * 6.283285;
	float	sampleAngle = random(CubeTexCoord());
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
		endPoint += TexCoord();
		float	samplingOcclusion = 0;
		for (int sampleNum = 1; sampleNum <= SAMPLES; ++sampleNum)
		{
			vec2	screenCoords = mix(TexCoord(), endPoint, sampleNum / float(SAMPLES));
			vec3	diff = WorldPosition(screenCoords) - WorldPosition(); 
			const vec3 v = normalize(diff); 
			const float d = length(diff) * RADIUS;
			samplingOcclusion += max(0.0, dot(WorldNormal(), v) - 0.025) * (1.0 / (1.0 + d)) * STRENGTH;
		}
		occlusion += samplingOcclusion / SAMPLES;
	}
	occlusion /= float(KERNEL_SIZE);
	SetAO(max(0, occlusion));
	//Out.Color.rgb = vec3(sampleAngle);
	//Out.Color.a = 1;
	//Out.Emissive = vec3(0);
}

)""