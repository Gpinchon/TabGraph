R""(
uniform vec3		envGammaCorrection = vec3(2.2);
uniform sampler2D	LastColor;
uniform sampler2D	LastNormal;
uniform sampler2D	LastDepth;

vec2 rotateUV(vec2 uv, float rotation, vec2 mid)
{
	return vec2(
		cos(rotation) * (uv.x - mid.x) + sin(rotation) * (uv.y - mid.y) + mid.x,
		cos(rotation) * (uv.y - mid.y) - sin(rotation) * (uv.x - mid.x) + mid.y
		);
}

uint ditherMatrix[] = uint[16](
	0,	8,	2,	10,
	12,	4,	14,	6,
	3,	11,	1,	9,
	15,	7,	13,	5
);

uint dither(ivec2 position) {
  int x = position.x % 4;
  int y = position.y % 4;
  int index = x + y * 4;
  return ditherMatrix[index % 16];
}

float vec2cross(in vec2 a, in vec2 b)
{
    return a.x * b.y - b.x * a.y;
}

bool lineSegmentIntersection(out float intersection, in vec2 o, in vec2 d, in vec2 a, in vec2 b)
{
	vec2	v1 = o - a;
	vec2	v2 = b - a;
	vec2	v3 = vec2(-d.y, d.x);
	float	t1 = vec2cross(v2, v1) / dot(v2, v3);
	float	t2 = dot(v1, v3) / dot(v2, v3);
	bool	hit = t1 >= 0 && t2 >= 0 && t2 <= 1;
	if (hit)
		intersection = t1;
	return hit;
}

vec4 sides[] = vec4[4](
	vec4(0, 0, 1, 0),
	vec4(1, 0, 1, 1),
	vec4(1, 1, 0, 1),
	vec4(0, 1, 0, 0)
);

bool	castRay(inout vec3 RayOrigin, in vec3 RayDir, in float stepOffset)
{
	float	distanceToBorder = 1.0;
	for (int side = 0; side < 4; side++)
	{
		if (lineSegmentIntersection(distanceToBorder, RayOrigin.xy, RayDir.xy,
					vec2(sides[side].x, sides[side].y), vec2(sides[side].z, sides[side].w)))
			break;
	}
	float	maxMipMaps = textureMaxLod(LastDepth);
	float	minMipMaps = 0;
	float	maxTries = REFLEXION_STEPS;
	vec2	pixelSize = 1 / vec2(textureSize(LastDepth, 0));
	float	step = (distanceToBorder / maxTries) + max(pixelSize.x, pixelSize.y);
	float 	compareTolerance = RayOrigin.z * step;
	//float 	compareTolerance = abs(RayOrigin.z) * step * 2;
	float	sampleDist = stepOffset * step + step;
	float	mipLevel = minMipMaps;
	for (float tries = 0; tries < maxTries && mipLevel >= minMipMaps; tries++)
	{
		vec3	curUV = RayOrigin + RayDir * sampleDist;
		float	sampleDepth = texelFetchLod(LastDepth, curUV.xy, int(mipLevel)).r;
		float	depthDiff = curUV.z - sampleDepth;
		bool	hit = abs(depthDiff) < compareTolerance;
		if (hit)
			mipLevel--;
		else {
			if (mipLevel < maxMipMaps)
				mipLevel++;
			sampleDist += step;
		}
		//mipLevel += Frag.Material.Roughness * (4.0 / maxTries);
	}
	return mipLevel < minMipMaps;
}

/* bool	castRay(inout vec3 RayOrigin, in vec3 RayDir, in float stepOffset)
{
	float	distanceToBorder = 1.0;
	for (int side = 0; side < 4; side++)
	{
		if (lineSegmentIntersection(distanceToBorder, RayOrigin.xy, RayDir.xy,
					vec2(sides[side].x, sides[side].y), vec2(sides[side].z, sides[side].w)))
			break;
	}
	float	minMipMaps = 0;
	float	maxTries = REFLEXION_STEPS;
	float	step = distanceToBorder / maxTries;
	float 	compareTolerance = RayOrigin.z * step;
	//float 	compareTolerance = abs(RayOrigin.z) * step * 2;
	float	sampleDist = stepOffset * step + step;
	float	mipLevel = minMipMaps;
	for (float tries = 0; tries < maxTries; tries++)
	{
		vec3	curUV = RayOrigin + RayDir * sampleDist;
		float	sampleDepth = texelFetchLod(LastDepth, curUV.xy, int(mipLevel)).r;
		float	depthDiff = curUV.z - sampleDepth;
		bool	hit = abs(depthDiff) < compareTolerance;
		if (hit && sampleDepth < 1) {
			RayOrigin = curUV;
			return true;
		}
		sampleDist += step;
		mipLevel += Frag.Material.Roughness * (4.0 / maxTries);
	}
	return false;
} */

uint ReverseBits32(uint bits)
{
	bits = ( bits << 16) | ( bits >> 16);
	bits = ( (bits & 0x00ff00ff) << 8 ) | ( (bits & 0xff00ff00) >> 8 );
	bits = ( (bits & 0x0f0f0f0f) << 4 ) | ( (bits & 0xf0f0f0f0) >> 4 );
	bits = ( (bits & 0x33333333) << 2 ) | ( (bits & 0xcccccccc) >> 2 );
	bits = ( (bits & 0x55555555) << 1 ) | ( (bits & 0xaaaaaaaa) >> 1 );
	return bits;
}

uint ReverseUIntBits( uint bits )
{
	bits = ( (bits & 0x33333333) << 2 ) | ( (bits & 0xcccccccc) >> 2 );
	bits = ( (bits & 0x55555555) << 1 ) | ( (bits & 0xaaaaaaaa) >> 1 );
	return bits;
}

vec2 Hammersley(uint Index, uint NumSamples, uvec2 Random)
{
	float E1 = fract( Index / float(NumSamples) + float( Random.x & 0xffff ) / (1<<16) );
	float E2 = float( ReverseBits32(Index) ^ Random.y ) * 2.3283064365386963e-10;
	return vec2( E1, E2 );
}

vec2 Hammersley(uint SampleIdx, uint SampleCnt)
{
    float u = float(SampleIdx) / float(SampleCnt);
    float v = float(ReverseBits32(SampleIdx)) * 2.3283064365386963e-10;
    return vec2(u, v);
}

uint MortonCode(uint x)
{
	x = (x ^ (x <<  2)) & 0x33333333;
	x = (x ^ (x <<  1)) & 0x55555555;
	return x;
}

float ClampedPow(float X, float Y)
{
	return pow(max(abs(X), 0.000001f), Y);
}

vec3 ImportanceSampleBlinn(vec2 E, float Roughness)
{
	float m = Roughness * Roughness;
	float n = max(0, 2 / (m * m) - 2);
	float Phi = 2 * PI * E.x;
	float CosTheta = ClampedPow(E.y, 1 / (n + 1));
	float SinTheta = sqrt(1 - CosTheta * CosTheta);
	//Spherical to Cartesian
	vec3 H;
	H.x = SinTheta * cos(Phi);
	H.y = SinTheta * sin(Phi);
	H.z = CosTheta;
	return H;
}

vec4	SSR()
{
	float	Depth = texelFetchLod(LastDepth, Frag.UV, 0).r;
	vec3	WSPos = ScreenToWorld(Frag.UV, Depth);
	vec3	WSViewDir = normalize(Camera.Position - WSPos);
	vec3	SSPos = vec3(Frag.UV, Depth);
	uint	FrameRandom = uint(Time * 1000.f) % 7 + 1;
	vec4	outColor = vec4(0);
	uvec2	PixelPos = ivec2(textureSize(LastDepth, 0) * Frag.UV);
	//Get the pixel Dithering Value and reverse Bits
	uint	Morton = MortonCode(PixelPos.x & 3) | ( MortonCode(PixelPos.y & 3) * 2 );
	uint	PixelIndex = ReverseUIntBits(Morton);
	uvec2	Random = uvec2(random(vec2(PixelPos + FrameRandom * uvec2(97, 71)))) * uvec2(0x3127352, 0x11229256);
	for( int i = 0; i < REFLEXION_SAMPLES; i++ ) {
		float	Offset = float((PixelIndex + ReverseUIntBits(FrameRandom + i * 117)) & 15);
		float	StepOffset = (Offset / 15.0);
		//Generate random normals using Hammersley distribution
		vec2	E = Hammersley(i, REFLEXION_SAMPLES, Random);
		//Compute Half vector using Blinn Importance Sampling
		//Project Half vector from Tangent space to World space using TBN Matrix
		vec3	H = TangentToWorld(ImportanceSampleBlinn(E, Frag.Material.Roughness).xyz);
		vec3	WSReflectionDir = 2 * dot(WSViewDir, H) * H - WSViewDir;
		//Create new Point and project it to compute screen-space ray direction
		//TODO : Find a better way to do this
		vec3	SSRDir = WorldToScreen(WSReflectionDir + WSPos).xyz - SSPos;
		if (castRay(SSPos, SSRDir, StepOffset))
		{
			float SSRParticipation = 1 - smoothstep(0.25, 0.50, dot(WSViewDir, WSReflectionDir));
			//SSRParticipation *= smoothstep(-0.17, 0.0, dot(texture(LastNormal, SSPos.xy, 0).xyz, -WSReflectionDir));
			//Attenuate reflection factor when getting closer to screen border
			SSRParticipation -= smoothstep(0, 1, pow(abs(SSPos.x * 2 - 1), SCREEN_BORDER_FACTOR));
			SSRParticipation -= smoothstep(0, 1, pow(abs(SSPos.y * 2 - 1), SCREEN_BORDER_FACTOR));
			SSRParticipation = clamp(SSRParticipation, 0, 1);
			vec4 SampleColor = vec4(sampleLod(LastColor, SSPos.xy, Frag.Material.Roughness + abs(Depth - SSPos.z)).rgb * SSRParticipation, SSRParticipation);
			SampleColor.rgb /= 1 + Luminance(SampleColor.rgb);
			outColor += SampleColor;
		}
	}
	outColor /= float(REFLEXION_SAMPLES);
	outColor.rgb /= 1 - Luminance(outColor.rgb);
	return outColor;
}

float	Env_Specular(in float NdV, in float roughness)
{
	float	alpha = roughness * roughness;
	float	alpha2 = alpha * alpha;
	float	den = (alpha2 - 1) + 1;
	float	D = alpha2 / (M_PI * den * den);
	float	k = alpha / 2.f;
	float	denom = NdV * (1.f - k) + k;
	float	G = NdV / denom;
	return (max(D * G, 0));
}

vec3	Fresnel(in float factor, in vec3 F0, in float roughness)
{
	return ((max(vec3(1 - roughness), F0)) * pow(max(0, 1 - factor), 5) + F0);
}

void	ApplyTechnique()
{
	vec3	EnvDiffuse = texture(Texture.Environment.Diffuse, Frag.CubeUV).rgb;

	Frag.Material.AO = 1 - Frag.Material.AO;
	
	vec3	V = normalize(Camera.Position - Frag.Position);
	vec3	N = Frag.Normal;
#ifdef TRANSPARENT
	float	NdV = dot(N, V);
	if (Frag.Material.Alpha < 1 && NdV < 0) {
		N = -N;
		NdV = -NdV;
	}
	else {
		NdV = max(0, dot(N, V));
	}
#else
	float	NdV = max(0, dot(N, V));
#endif //TRANSPARENT
	vec3	R = reflect(V, N);

	vec2	brdf = BRDF(NdV, Frag.Material.Roughness);

	vec3	diffuse = Frag.Material.AO * (sampleLod(Texture.Environment.Diffuse, -N, Frag.Material.Roughness + 0.9).rgb
		+ texture(Texture.Environment.Irradiance, -N).rgb);
	vec3	reflection = sampleLod(Texture.Environment.Diffuse, R, Frag.Material.Roughness * 2.f).rgb;
	vec3	specular = texture(Texture.Environment.Irradiance, R).rgb;
	vec3	reflection_spec = reflection;

	float	brightness = 0;

	if (Frag.Material.Alpha == 0) {
		#ifdef TRANSPARENT
		return ;
		#else
		Out.Color = vec4(EnvDiffuse, 1);
		brightness = Luminance(pow(Out.Color.rgb, envGammaCorrection));
		Out.Emitting = max(vec3(0), (Out.Color.rgb - 0.8) * min(1, brightness));
		return ;
		#endif //TRANSPARENT
	}
	vec3	fresnel = min(vec3(0.5), Fresnel(NdV, Frag.Material.Specular, Frag.Material.Roughness));
	reflection *= fresnel;
	brightness = Luminance(pow(reflection_spec, envGammaCorrection));
	reflection_spec *= brightness * min(fresnel + 0.5, fresnel * Env_Specular(NdV, Frag.Material.Roughness));
	specular *= fresnel * brdf.x + mix(vec3(1), fresnel, Frag.Material.Metallic) * brdf.y;
	diffuse *= Frag.Material.Albedo.rgb * (1 - Frag.Material.Metallic);

	float	alpha = Frag.Material.Alpha + max(specular.r, max(specular.g, specular.b));
	alpha = min(1, alpha);

	vec3	envReflection = (specular + reflection_spec + reflection) * alpha;
	vec4	ssrResult = SSR();
	Out.Color.rgb += mix(envReflection, ssrResult.xyz * fresnel, ssrResult.w);
	Out.Color.rgb += (diffuse + Frag.Material.Emitting) * alpha;
	Out.Color.a = 1;
	Out.Emitting.rgb += max(vec3(0), Out.Color.rgb - 1) + Frag.Material.Emitting;
}

)""
