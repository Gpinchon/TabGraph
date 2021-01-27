R""(

uniform sampler2D	LastColor;

#if SSR_QUALITY == 1
#define NumSteps 8
#define NumRays 1
#elif SSR_QUALITY == 2
#define NumSteps 16
#define NumRays 1
#elif SSR_QUALITY == 3
#define NumSteps 8
#define NumRays 4
#else // SSR_QUALITY == 4
#define NumSteps 8
#define NumRays 8
#endif

uint ReverseBits32(uint bits)
{
	bits = ( bits << 16) | ( bits >> 16);
	bits = ( (bits & 0x00ff00ff) << 8 ) | ( (bits & 0xff00ff00) >> 8 );
	bits = ( (bits & 0x0f0f0f0f) << 4 ) | ( (bits & 0xf0f0f0f0) >> 4 );
	bits = ( (bits & 0x33333333) << 2 ) | ( (bits & 0xcccccccc) >> 2 );
	bits = ( (bits & 0x55555555) << 1 ) | ( (bits & 0xaaaaaaaa) >> 1 );
	return bits;
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

vec4	LastWorldToView(in vec3 position)
{
	return PrevCamera.Matrix.View * vec4(position, 1);
}

vec4	LastViewToClip(in vec4 position)
{
	return PrevCamera.Matrix.Projection * position;
}

/** Returns the World position and the scene depth in world units */
vec4	LastWorldToScreen(in vec3 position)
{
	return ClipToScreen(LastViewToClip(LastWorldToView(position)));
}

vec3	GetLastUVz(in vec3 UVz)
{
	vec3 worldPos = ScreenToWorld(UVz.xy, UVz.z);
	UVz = LastWorldToScreen(worldPos).xyz;
	return UVz;
}

vec3	GetLastUVz(in vec2 UV, in float z)
{
	return GetLastUVz(vec3(UV, z));
}

vec4 SampleDepthTexture(float Level, vec4 SampleUV0, vec4 SampleUV1 )
{
	vec4 SampleDepth;
	// SampleUV{0,1}.{xy,zw} should already be in the HZB UV frame using HZBUvFactorAndInvFactor.xy
	SampleDepth.x = textureLod(Texture.Geometry.Depth, SampleUV0.xy, (Level)).r;
	SampleDepth.y = textureLod(Texture.Geometry.Depth, SampleUV0.zw, (Level)).r;
	SampleDepth.z = textureLod(Texture.Geometry.Depth, SampleUV1.xy, (Level)).r;
	SampleDepth.w = textureLod(Texture.Geometry.Depth, SampleUV1.zw, (Level)).r;
	return SampleDepth;
}

//Simplified AABB-Ray Intersection
// See : https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
void IntersectClippingBounds(const in vec3 origin, const in vec3 dir, out vec3 coord)
{
	vec3 vmin = vec3(-1, -1, -1);
	vec3 vmax = vec3(1, 1, 1);
	float t[7];
	t[0] = (vmin.x - origin.x)/dir.x;
	t[1] = (vmax.x - origin.x)/dir.x;
	t[2] = (vmin.y - origin.y)/dir.y;
	t[3] = (vmax.y - origin.y)/dir.y;
	t[4] = (vmin.z - origin.z)/dir.z;
	t[5] = (vmax.z - origin.z)/dir.z;
	t[6] = min(min(max(t[0], t[1]), max(t[2], t[3])), max(t[4], t[5]));
	coord = origin + dir * t[6];
}

#pragma optionNV (unroll all)

vec4	castRay(vec3 R, float StepOffset)
{
	const float Step = 1 / float(NumSteps + 1);
	vec4 RayStartClip	= WorldToClip(WorldPosition());
	vec4 RayEndClip	= WorldToClip(WorldPosition() + R * RayStartClip.w);
	vec3 RayStartScreen = RayStartClip.xyz / RayStartClip.w;
	vec3 RayEndScreen = RayEndClip.xyz / RayEndClip.w;
	vec3 RayDirScreen = normalize(RayEndScreen - RayStartScreen);
	//Clip ray agains Normalized Device Coordinate's bounding box
	IntersectClippingBounds(RayStartScreen, RayDirScreen, RayEndScreen);
	vec3 RayStartUVz = RayStartScreen * 0.5 + 0.5;
	vec3 RayEndUVz = RayEndScreen * 0.5 + 0.5;
	vec3 RayStepUVz = (RayEndUVz - RayStartUVz);
	const float CompareTolerance = abs(RayStepUVz.z) * Step * 2;
	vec4 Result = vec4( 0, 0, 0, 1 );
	float LastDiff = 0;
	/*
	vec4 SampleTime = (StepOffset * Step + Step) * vec4( 1, 2, 3, 4 );
	float Level = 0;
	for (int i = 0; i < NumSteps; i += 4) {
		vec4 SampleUV0 = RayStartUVz.xyxy + RayStepUVz.xyxy * SampleTime.xxyy;
		vec4 SampleUV1 = RayStartUVz.xyxy + RayStepUVz.xyxy * SampleTime.zzww;
		vec4 SampleZ   = RayStartUVz.zzzz + RayStepUVz.zzzz * SampleTime;
		vec4 SampleDepth = SampleDepthTexture(Level, SampleUV0, SampleUV1);
		vec4 DepthDiff = SampleDepth - SampleZ;
		bvec4 Hit = lessThan(abs(-DepthDiff - CompareTolerance), vec4(CompareTolerance));
		if (any(Hit))
		{
			float closestHit = 1;
			int closestHitIndex = 0;
			for (int j = 0; j < 4; ++j)
			{
				if (Hit[j] && abs(DepthDiff[j]) < closestHit)
				{
					closestHit = abs(DepthDiff[j]);
					closestHitIndex = j;
				}
			}
			vec3 HitUVz = RayStartUVz + RayStepUVz * SampleTime[closestHitIndex];
			Result = vec4(HitUVz, SampleTime[closestHitIndex] * SampleTime[closestHitIndex]);
			break;
		}
		LastDiff = DepthDiff.w;
		Level += Alpha() * (16.0 * Step);
		SampleTime += 4.0 * Step;
	}
	*/
	float SampleTime = StepOffset * Step + Step;
	for (int i = 0; i < NumSteps; ++i)
	{
		vec3 UVz = RayStartUVz + RayStepUVz * SampleTime;
		float Level = Alpha() * (i * 4.0 / NumSteps);
		float SampleZ = textureLod(Texture.Geometry.Depth, UVz.xy, Level).r;
		float DepthDiff = SampleZ - UVz.z;
		if (abs(-DepthDiff - CompareTolerance) < CompareTolerance)
		{
			// Find more accurate hit using line segment intersection
			float TimeLerp = clamp( LastDiff / (LastDiff - DepthDiff), 0, 1);
			float IntersectTime = SampleTime + TimeLerp * Step - Step;
			UVz = RayStartUVz + RayStepUVz * IntersectTime;
			Result = vec4(UVz, IntersectTime * IntersectTime);
			break;
		}
		LastDiff = DepthDiff;
		SampleTime += Step;
	}
	return Result;
}

vec4	SampleScreenColor(vec3 UVz)
{
	UVz = GetLastUVz(UVz);
	return texture(LastColor, UVz.xy, 0);
}

float GetRoughnessFade()
{
	// mask SSR to reduce noise and for better performance, roughness of 0 should have SSR, at MaxRoughness we fade to 0
	return min(sqrt(Alpha()) * ROUGHNESSMASKSCALE + 2, 1.0);
}

//Use Rodrigues' rotation formula to rotate v about k
vec3 rotateAround(vec3 v, vec3 k, float angle)
{
	return v * cos(angle) + cross(k, v) * sin(angle) + k * dot(k, v) * (1 - cos(angle));
}

float InterleavedGradientNoise(vec2 uv, float FrameId)
{
	// magic values are found by experimentation
	uv += FrameId * (vec2(47, 17) * 0.695f);

    const vec3 magic = vec3( 0.06711056f, 0.00583715f, 52.9829189f );
    return fract(magic.z * fract(dot(uv, magic.xy)));
}

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

void	SSR()
{
	//if (Opacity() < 1)
	//	return;
	vec3	V = normalize(WorldPosition() - Camera.Position);
	float	SceneDepth = WorldToClip(WorldPosition()).w;
	uvec2	PixelPos = ivec2(gl_FragCoord.xy);
	uint	frameIndex = FrameNumber % 8;
	float	Noise =	InterleavedGradientNoise(gl_FragCoord.xy, frameIndex);
	uvec3	Random = Rand3DPCG16(ivec3(PixelPos, frameIndex));
	//float	sampleAngle = randomAngle(vec3(Random));
	float samplesWeight = 0;
	out_1 = vec4(0);
	for(int i = 0; i < NumRays; i++ ) {
		float	StepOffset = Noise;
		StepOffset -= 0.5;
		//Generate random normals using Hammersley distribution
		vec2	E = Hammersley(i, NumRays, Random.xy);
		//Compute Half vector using GGX Importance Sampling
		//Project Half vector from Tangent space to World space
		vec3	H = ImportanceSampleGGX(E, WorldNormal(), Alpha());
		//H = rotateAround(H, WorldNormal(), Noise * 6.2831853);
		vec3	R = reflect(V, H);
		vec4	SSRResult = castRay(R, StepOffset);
		if (SSRResult.w < 1)
		{
			float SSRParticipation = clamp(4 - 4 * SSRResult.w, 0, 1);
			//Attenuate reflection factor when getting closer to screen border
			vec4 SampleColor = vec4(SampleScreenColor(SSRResult.xyz).rgb, SSRParticipation);
			SampleColor.rgb /= 1 + Luminance(SampleColor.rgb);
			samplesWeight += SSRParticipation;
			out_1 += SampleColor * SSRParticipation;
		}
	}
	if (samplesWeight == 0)
		return;
	out_1 /= samplesWeight;
	out_1.rgb /= 1 - Luminance(out_1.rgb);
	out_1 *= GetRoughnessFade();
}

)""