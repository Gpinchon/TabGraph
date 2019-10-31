R""(
uniform sampler2D	LastColor;
uniform sampler2D	LastNormal;
uniform sampler2D	LastDepth;

float vec2cross(in vec2 a, in vec2 b)
{
    return a.x * b.y - b.x * a.y;
}

uint MortonCode(uint x)
{
	x = (x ^ (x <<  2)) & 0x33333333;
	x = (x ^ (x <<  1)) & 0x55555555;
	return x;
}

float PseudoRandom(vec2 xy)
{
	vec2 pos = fract(xy / 128.0f) * 128.0f + vec2(-64.340622f, -72.465622f);
	
	// found by experimentation
	return fract(dot(pos.xyx * pos.xyy, vec3(20.390625f, 60.703125f, 2.4281209f)));
}

uint ReverseBits32(uint bits)
{
	bits = ( bits << 16) | ( bits >> 16);
	bits = ( (bits & 0x00ff00ff) << 8 ) | ( (bits & 0xff00ff00) >> 8 );
	bits = ( (bits & 0x0f0f0f0f) << 4 ) | ( (bits & 0xf0f0f0f0) >> 4 );
	bits = ( (bits & 0x33333333) << 2 ) | ( (bits & 0xcccccccc) >> 2 );
	bits = ( (bits & 0x55555555) << 1 ) | ( (bits & 0xaaaaaaaa) >> 1 );
	return bits;
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
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

uint ReverseUIntBits( uint bits )
{
	bits = ( (bits & 0x33333333) << 2 ) | ( (bits & 0xcccccccc) >> 2 );
	bits = ( (bits & 0x55555555) << 1 ) | ( (bits & 0xaaaaaaaa) >> 1 );
	return bits;
}

vec4 SampleDepthTexture(float Level, vec4 SampleUV0, vec4 SampleUV1 )
{
	vec4 SampleDepth;
	// SampleUV{0,1}.{xy,zw} should already be in the HZB UV frame using HZBUvFactorAndInvFactor.xy
	SampleDepth.x = texelFetchLod(LastDepth, SampleUV0.xy, (Level)).r;
	SampleDepth.y = texelFetchLod(LastDepth, SampleUV0.zw, (Level)).r;
	SampleDepth.z = texelFetchLod(LastDepth, SampleUV1.xy, (Level)).r;
	SampleDepth.w = texelFetchLod(LastDepth, SampleUV1.zw, (Level)).r;
	return SampleDepth;
}

vec4 sides[] = vec4[4](
	vec4(0, 0, 1, 0),
	vec4(1, 0, 1, 1),
	vec4(1, 1, 0, 1),
	vec4(0, 1, 0, 0)
);

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

bool	castRay(inout vec3 RayStartUVz, in vec3 RayDir, in float StepOffset)
{
	float	distanceToBorder = 1.0;
	const vec2	UVDir = normalize(RayDir.xy);
	for (int side = 0; side < 4; side++)
	{
		if (lineSegmentIntersection(distanceToBorder, RayStartUVz.xy, UVDir, sides[side].xy, sides[side].zw))
			break;
	}
	const float	minMipMaps = 0;
	const float	maxMipMaps = textureMaxLod(LastDepth);
	const float	maxTries = REFLEXION_STEPS;
	const float	Step = 1.0 / maxTries;
	const vec3	RayEnd = RayStartUVz + RayDir * distanceToBorder;
	const vec3	RayStepUVz = (RayEnd - RayStartUVz) * Step;
	//const float	RayStepUVz = distanceToBorder / maxTries;
	const float 	CompareTolerance = abs(RayStepUVz.z);
	//const float 	CompareTolerance = min(1.0 / maxTries, RayStartUVz.z * RayStepUVz.z);
	//const float	CompareTolerance = max(abs(RayStepUVz.z)/* 0.06 */, (RayStartUVz.z - RayEnd.z) * Step * 4);
	//const float 	CompareTolerance = RayStartUVz.z * abs(RayStepUVz.z) * 2;
	//const float 	CompareTolerance = abs(RayStartUVz.z) * RayStepUVz.z);
	float	mipLevel = minMipMaps;
	vec3	RayUVz = RayStartUVz + RayStepUVz * StepOffset;
	float	LastDiff = 0;
	for (int tries = 0; tries < REFLEXION_STEPS; tries += 4)
	{
		// Vectorized to group fetches
		vec4	SampleUV0 = RayUVz.xyxy + RayStepUVz.xyxy * vec4( 1, 1, 2, 2 );
		vec4	SampleUV1 = RayUVz.xyxy + RayStepUVz.xyxy * vec4( 3, 3, 4, 4 );
		vec4	SampleZ   = RayUVz.zzzz + RayStepUVz.zzzz * vec4( 1, 2, 3, 4 );
		vec4	sampleDepth = SampleDepthTexture(mipLevel * maxMipMaps, SampleUV0, SampleUV1);
		vec4	DepthDiff = SampleZ - sampleDepth;
		bvec4	Hit = greaterThan(SampleZ, sampleDepth);// || abs(depthDiff) < CompareTolerance;
		if(any(Hit))
		{
			float DepthDiff0 = DepthDiff[2];
			float DepthDiff1 = DepthDiff[3];
			float MinTime = 3;
			if(Hit[2])
			{
				DepthDiff0 = DepthDiff[1];
				DepthDiff1 = DepthDiff[2];
				MinTime = 2;
			}
			if(Hit[1])
			{
				DepthDiff0 = DepthDiff[0];
				DepthDiff1 = DepthDiff[1];
				MinTime = 1;
			}
			if(Hit[0])
			{
				DepthDiff0 = LastDiff;
				DepthDiff1 = DepthDiff[0];
				MinTime = 0;
			}
			// Find more accurate hit using line segment intersection
			float	TimeLerp = clamp(DepthDiff0 / (DepthDiff0 - DepthDiff1), 0, 1);
			float	IntersectTime = MinTime + TimeLerp;
			vec3	HitUVz = RayUVz + RayStepUVz * IntersectTime;
			if (texelFetchLod(LastDepth, HitUVz.xy, 0).r < 1) {
				RayStartUVz = HitUVz;
				return true;
			}
		}
		LastDiff = DepthDiff.w;
		mipLevel += Frag.Material.Roughness * (16.0 / maxTries);
		RayUVz += 4 * RayStepUVz;
	}
	return false;
}

vec4	SSR()
{
	float	SSDepth = texelFetchLod(LastDepth, Frag.UV, 0).r;
	vec3	SSPos = vec3(Frag.UV, SSDepth);
	vec3	WSPos = ScreenToWorld(Frag.UV, SSDepth);
	vec3	WSNormal = texture(LastNormal, Frag.UV, 0).xyz;
	vec3	WSViewDir = normalize(Camera.Position - WSPos);
	uint	FrameRandom = uint(Time * 1000.f) % 7 + 1;
	vec4	outColor = vec4(0);
	uvec2	PixelPos = ivec2(textureSize(LastDepth, 0) * Frag.UV);
	//Get the pixel Dithering Value and reverse Bits
	uint	Morton = MortonCode(PixelPos.x & 3) | ( MortonCode(PixelPos.y & 3) * 2 );
	uint	PixelIndex = ReverseUIntBits(Morton);
	uvec2	Random = uvec2(PseudoRandom(vec2(PixelPos + FrameRandom * uvec2(97, 71)))) * uvec2(0x3127352, 0x11229256);
	for( int i = 0; i < REFLEXION_SAMPLES; i++ ) {
		float	Offset = float((PixelIndex + ReverseUIntBits(FrameRandom + i * 117)) & REFLEXION_SAMPLES);
		float	StepOffset = (Offset / float(REFLEXION_SAMPLES));
		//Generate random normals using Hammersley distribution
		vec2	E = Hammersley(i, REFLEXION_SAMPLES, Random);
		//Compute Half vector using GGX Importance Sampling
		//Project Half vector from Tangent space to World space
		vec3	H = ImportanceSampleGGX(E, WSNormal, Frag.Material.Roughness);
		vec3	WSReflectionDir = -reflect(WSViewDir, H);
		//Create new Point and project it to compute screen-space ray direction
		//TODO : Find a better way to do this
		vec3	SSRDir = WorldToScreen(WSPos + WSReflectionDir).xyz - SSPos;
		SSRDir = normalize(SSRDir);
		if (castRay(SSPos, SSRDir, StepOffset))
		{
			float SSRParticipation = clamp(-dot(WSViewDir, WSReflectionDir) + 0.25, 0, 1);//1 - smoothstep(0.25, 0.50, dot(WSViewDir, WSReflectionDir));
			//SSRParticipation *= smoothstep(-0.17, 0.0, dot(texture(LastNormal, SSPos.xy, 0).xyz, -WSReflectionDir));
			//Attenuate reflection factor when getting closer to screen border
			SSRParticipation -= smoothstep(0, 1, pow(abs(SSPos.x * 2 - 1), SCREEN_BORDER_FACTOR));
			SSRParticipation -= smoothstep(0, 1, pow(abs(SSPos.y * 2 - 1), SCREEN_BORDER_FACTOR));
			SSRParticipation = clamp(SSRParticipation, 0, 1);
			vec4 SampleColor = vec4(sampleLod(LastColor, SSPos.xy, Frag.Material.Roughness + abs(SSDepth - SSPos.z)).rgb * SSRParticipation, SSRParticipation);
			SampleColor.rgb /= 1 + Luminance(SampleColor.rgb);
			outColor += SampleColor;
		}
	}
	outColor /= float(REFLEXION_SAMPLES);
	outColor.rgb /= 1 - Luminance(outColor.rgb);
	return outColor;
}

void	ApplyTechnique() {
	Out.Color = SSR();
}

)""