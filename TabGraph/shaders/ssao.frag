R""(
#define STRENGTH 1.f
#define RADIUS 0.05f

#if SSAO_QUALITY == 1
#define SAMPLENBR 8
#elif SSAO_QUALITY == 2
#define SAMPLENBR 16
#elif SSAO_QUALITY == 3
#define SAMPLENBR 32
#else //SSAO_QUALITY == 4
#define SAMPLENBR 64
#endif


float InterleavedGradientNoise(vec2 uv, float FrameId)
{
	// magic values are found by experimentation
	uv += FrameId * (vec2(47, 17) * 0.695f);

    const vec3 magic = vec3( 0.06711056f, 0.00583715f, 52.9829189f );
    return fract(magic.z * fract(dot(uv, magic.xy)));
}

vec2 Hammersley(uint SampleIdx, uint SampleCnt)
{
    float u = float(SampleIdx) / float(SampleCnt);
    float v = float(bitfieldReverse(SampleIdx)) * 2.3283064365386963e-10;
    return vec2(u, v);
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float alpha)
{	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (alpha*alpha - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
	
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

void	SSAO()
{
	float occlusion = 0.f;
	const vec3 N = WorldNormal();
	const vec3 P = WorldPosition();
    const uint frameIndex = FrameNumber % 8;
	const float noise = InterleavedGradientNoise(gl_FragCoord.xy, frameIndex) * 2 - 1;
	for (int i = 0; i < SAMPLENBR; ++i)
	{
		vec2 E = Hammersley(i, SAMPLENBR) * vec2(M_PI, 2 * M_PI);
		E.y += noise;
		vec2 sE= vec2(cos(E.y), sin(E.y)) * RADIUS * cos(E.x);
		vec2 screenCoords = ScreenTexCoord() + sE;
		vec3 V = WorldPosition(screenCoords) - P;
        float d = length(V);
        V /= d;
        d *= RADIUS;
		occlusion += max(0.0, dot(N, V) - 0.025) * (1.0 / (1.0 + d));
	}
	occlusion /= float(SAMPLENBR);
	occlusion *= STRENGTH;
	SetAO(max(0, occlusion));
}

)""