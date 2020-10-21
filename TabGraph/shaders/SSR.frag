R""(

uniform sampler2D	LastColor;
uniform ivec2		FrameBufferResolution;
uniform mat4		LastViewMatrix;
uniform mat4		LastProjectionMatrix;

#if SSR_QUALITY == 1
#define NumSteps 4
#define NumRays 2
#elif SSR_QUALITY == 2
#define NumSteps 8
#define NumRays 2
#elif SSR_QUALITY == 3
#define NumSteps 8
#define NumRays 4
#else // SSR_QUALITY == 4
#define NumSteps 12
#define NumRays 12
#endif

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

uint ReverseUIntBits( uint bits )
{
	bits = ( (bits & 0x33333333) << 2 ) | ( (bits & 0xcccccccc) >> 2 );
	bits = ( (bits & 0x55555555) << 1 ) | ( (bits & 0xaaaaaaaa) >> 1 );
	return bits;
}

vec4	LastWorldToView(in vec3 position)
{
	return LastViewMatrix * vec4(position, 1);
}

vec4	LastViewToClip(in vec4 position)
{
	return LastProjectionMatrix * position;
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

void	SSR()
{
	vec3	V = normalize(WorldPosition() - Camera.Position);
	uint	FrameRandom = FrameNumber % 8 * 1551;
	vec4	outColor = vec4(0);
	float	SceneDepth = WorldToClip(WorldPosition()).w;
	uvec2	PixelPos = ivec2(FrameBufferResolution * TexCoord());
	//Get the pixel Dithering Value and reverse Bits
	uint	Morton = MortonCode(PixelPos.x & 3) | ( MortonCode(PixelPos.y & 3) * 2);
	uint	PixelIndex = ReverseUIntBits(Morton);
	uvec2	Random = uvec2(PseudoRandom(vec2(PixelPos + FrameRandom * uvec2(97, 71)))) * uvec2(0x3127352, 0x11229256);
	float	sampleAngle = randomAngle(WorldPosition());
	for( int i = 0; i < NumRays; i++ ) {
		uint	Offset = (PixelIndex + ReverseUIntBits(FrameRandom + i * 117)) & 15;
		float	StepOffset = Offset / 15.f;
		StepOffset -= 0.5;
		//Generate random normals using Hammersley distribution
		vec2	E = Hammersley(i, NumRays, Random);
		//Compute Half vector using GGX Importance Sampling
		//Project Half vector from Tangent space to World space
		vec3	H = ImportanceSampleGGX(E, WorldNormal(), Alpha());
		H = rotateAround(H, WorldNormal(), sampleAngle);
		vec3	R = reflect(V, H);
		vec4	SSRResult = castRay(R, StepOffset);
		if (SSRResult.w < 1)
		{
			float SSRParticipation = clamp(4 - 4 * SSRResult.w, 0, 1);
			//Attenuate reflection factor when getting closer to screen border
			vec4 SampleColor = vec4(SampleScreenColor(SSRResult.xyz).rgb, SSRParticipation);
			SampleColor.rgb /= 1 + Luminance(SampleColor.rgb);
			outColor += SampleColor * SSRParticipation;
		}
	}
	outColor /= NumRays;
	outColor.rgb /= 1 - Luminance(outColor.rgb);
	outColor *= GetRoughnessFade();
	SetBackColor(outColor);
}
)""