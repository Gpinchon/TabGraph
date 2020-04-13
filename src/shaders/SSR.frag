R""(
uniform sampler2D	LastColor;
uniform sampler2D	LastNormal;
uniform sampler2D	LastDepth;
uniform mat4 LastInvViewMatrix;
uniform mat4 LastInvProjectionMatrix;
uniform mat4 LastViewMatrix;
uniform mat4 LastProjectionMatrix;

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
	SampleDepth.x = textureLod(Texture.Depth, SampleUV0.xy, (Level)).r;
	SampleDepth.y = textureLod(Texture.Depth, SampleUV0.zw, (Level)).r;
	SampleDepth.z = textureLod(Texture.Depth, SampleUV1.xy, (Level)).r;
	SampleDepth.w = textureLod(Texture.Depth, SampleUV1.zw, (Level)).r;
	return SampleDepth;
}

vec4	castRay(vec3 R, float Roughness, float SceneDepth, vec3 PositionTranslatedWorld, int NumSteps, float StepOffset)
{
	const float Step = 1.0 / (NumSteps);
	vec4 RayStartClip	= WorldToClip(PositionTranslatedWorld);
	vec4 RayEndClip	= WorldToClip(PositionTranslatedWorld + R * SceneDepth);

	vec3 RayStartScreen = RayStartClip.xyz / RayStartClip.w;
	vec3 RayEndScreen = RayEndClip.xyz / RayEndClip.w;
	vec3 RayStartUVz = RayStartScreen * 0.5 + 0.5;
	vec3 RayEndUVz = RayEndScreen * 0.5 + 0.5;
	
	// Make sure the RayStepScreen has a length of 2 in order to have the end point ouside of the viewport
	//vec3 RayStepScreen = ( RayEndScreen - RayStartScreen ) / float(NumSteps);
	vec3 RayStepScreen = ( RayEndScreen - RayStartScreen );// / (0.5 * length( RayEndScreen.xy - RayStartScreen.xy ));
	StepOffset *= (1 + RayStartUVz.z);
	vec3 RayStepUVz = RayStepScreen * (1 + RayStartUVz.z);
	//vec3 RayStepUVz = vec3( RayStepScreen.xy * vec2( 0.5, -0.5 ) + 0.5, RayStepScreen.z );
	//RayStartUVz.xy *= HZBUvFactorAndInvFactor.xy;
	//RayStepUVz.xy *= HZBUvFactorAndInvFactor.xy;

	// *2 to get less morie pattern in extreme cases, larger values make object appear not grounded in reflections
	const float CompareTolerance = abs(RayStepUVz.z) * Step * 4;

	// avoid bugs with early returns inside of loops on certain platform compilers.
	vec4 Result = vec4( 0, 0, 0, 1 );
	float LastDiff = 0;
	float Level = 0;

	vec4 SampleTime = (StepOffset + vec4(1, 2, 3, 4)) * Step;
	//RayStartUVz = RayStartUVz + RayStepUVz * StepOffset;

	for( int i = 0; i < NumSteps; i += 4 )
	{
		// Vectorized to group fetches
		vec4 SampleUV0 = RayStartUVz.xyxy + RayStepUVz.xyxy * SampleTime.xxyy;
		vec4 SampleUV1 = RayStartUVz.xyxy + RayStepUVz.xyxy * SampleTime.zzww;
		vec4 SampleZ   = RayStartUVz.zzzz + RayStepUVz.zzzz * SampleTime;
		
		// Use lower res for farther samples
		vec4 SampleDepth = SampleDepthTexture(Level, SampleUV0, SampleUV1 );

		vec4 DepthDiff1 = SampleZ - SampleDepth;
		//bvec4 Hit = lessThan(vec4(SampleZ), vec4(SampleDepth)) || lessThan(abs(-DepthDiff1 - CompareTolerance), vec4(CompareTolerance));;
		bvec4	Hit = lessThan(abs(-DepthDiff1 - CompareTolerance), vec4(CompareTolerance));

		if( any( Hit ) )
		{
			// Find more accurate hit using line segment intersection
			vec4 DepthDiff0 = vec4( LastDiff, DepthDiff1.xyz );
			vec4 TimeLerp = clamp( DepthDiff0 / (DepthDiff0 - DepthDiff1), 0, 1);
			vec4 IntersectTime = SampleTime + (TimeLerp - 1) / (NumSteps + 1);
			vec4 HitTime = mix(IntersectTime, vec4(1), Hit);

			// Take closest hit
			HitTime.xy = min( HitTime.xy, HitTime.zw );
			float MinHitTime = min( HitTime.x, HitTime.y );
			
			vec3 HitUVz = RayStartUVz + RayStepUVz * MinHitTime;

			Result = vec4( HitUVz, MinHitTime );
			break;
		}

		LastDiff = DepthDiff1.w;
		Level += Roughness * (textureMaxLod(LastDepth) / NumSteps);
		SampleTime += 4.0 / float(NumSteps);
	}
	//Result.xy *= HZBUvFactorAndInvFactor.zw;
	//Result.xy = Result.xy * vec2( 2, 2 ) + vec2( -1, -1 );
	//Result.xy = Result.xy * View.ScreenPositionScaleBias.xy + View.ScreenPositionScaleBias.wz;
	return Result;
}

vec4	SampleScreenColor(vec3 UVz)
{
	UVz = GetLastUVz(UVz);
	return texture(LastColor, UVz.xy, 0);
}

#define MAXROUGHNESS 0.5
#define QUALITY 3

float ComputeRoughnessMaskScale()
{
	
	float MaxRoughness = clamp(MAXROUGHNESS, 0.01f, 1.0f);

	// f(x) = x * Scale + Bias
	// f(MaxRoughness) = 0
	// f(MaxRoughness/2) = 1

	float RoughnessMaskScale = -2.0f / MaxRoughness;
	return RoughnessMaskScale * (QUALITY < 3 ? 2.0f : 1.0f);
}

float GetRoughnessFade()
{
	// mask SSR to reduce noise and for better performance, roughness of 0 should have SSR, at MaxRoughness we fade to 0
	return min(Frag.Material.Roughness * ComputeRoughnessMaskScale() + 2, 1.0);
}

vec4	SSR()
{
	//vec3	SSPos = vec3(Frag.UV, Frag.Depth);
	vec3	WSPos = ScreenToWorld(Frag.UV, Frag.Depth);
	vec3	WSNormal = texture(Texture.Normal, Frag.UV, 0).xyz;
	vec3	WSViewDir = normalize(WSPos - Camera.Position);
	//uint	FrameRandom = uint(randomAngle(Frag.Position) * 1000) % 8 * 1551;
	//uint	FrameRandom = uint(randomAngle(Frag.Position) * 1000) + uint(Time * 1000.f) % 7 + 1;
	uint	FrameRandom = FrameNumber % 8 * 1551;
	vec4	outColor = vec4(0);
	float	SceneDepth = WorldToClip(WSPos).w;
	uvec2	PixelPos = ivec2(textureSize(Texture.Depth, 0) * Frag.UV.xy);
	//Get the pixel Dithering Value and reverse Bits
	uint	Morton = MortonCode(PixelPos.x & 3) | ( MortonCode(PixelPos.y & 3) * 2 );
	uint	PixelIndex = ReverseUIntBits(Morton);
	uvec2	Random = uvec2(PseudoRandom(vec2(PixelPos + FrameRandom * uvec2(97, 71)))) * uvec2(0x3127352, 0x11229256);
	float NumSamples = 0;
	//Frag.Material.Roughness *= Frag.Material.Roughness;
	for( int i = 0; i < REFLEXION_SAMPLES; i++ ) {
		uint	Offset = (PixelIndex + ReverseUIntBits(FrameRandom + i * 117)) & 15;
		float	StepOffset = Offset / 15.f;
		StepOffset -= 0.5;
		//Generate random normals using Hammersley distribution
		vec2	E = Hammersley(i, REFLEXION_SAMPLES, Random);
		//Compute Half vector using GGX Importance Sampling
		//Project Half vector from Tangent space to World space
		vec3	H = ImportanceSampleGGX(E, WSNormal, Frag.Material.Roughness);
		vec3	WSReflectionDir = reflect(WSViewDir, H);
		//Create new Point and project it to compute screen-space ray direction
		//TODO : Find a better way to do this
		
		//vec3	SSREnd = WorldToScreen(WSPos + WSReflectionDir * SceneDepth).xyz;
		vec4	SSRResult = castRay(WSReflectionDir, Frag.Material.Roughness, SceneDepth, WSPos, REFLEXION_STEPS, StepOffset);
		//vec4	SSRResult = castRay(SSPos, SSREnd, StepOffset);
		if (SSRResult.w < 1)
		{
			float SSRParticipation = 1;
			//float SSRParticipation = clamp(-dot(WSViewDir, WSReflectionDir) + 0.25, 0, 1);
			//SSRParticipation *= smoothstep(-0.17, 0.0, dot(texture(Texture.Normal, SSPos.xy, 0).xyz, -WSReflectionDir));
			//float SSRParticipation = 1;
			//Attenuate reflection factor when getting closer to screen border
			SSRParticipation -= smoothstep(0, 1, pow(abs(SSRResult.x * 2 - 1), SCREEN_BORDER_FACTOR));
			SSRParticipation -= smoothstep(0, 1, pow(abs(SSRResult.y * 2 - 1), SCREEN_BORDER_FACTOR));
			SSRParticipation = clamp(SSRParticipation, 0, 1);
			vec4 SampleColor = vec4(SampleScreenColor(SSRResult.xyz).rgb * SSRParticipation, SSRParticipation);
			//vec4 SampleColor = vec4(texture(LastColor, SSRResult.xy, 0).rgb * SSRParticipation, SSRParticipation);
			//vec4 SampleColor = vec4(sampleLod(LastColor, SSRResult.xy, abs(Frag.Depth - SSRResult.z)).rgb * SSRParticipation, SSRParticipation);
			SampleColor *= clamp( 4 - 4 * SSRResult.w, 0, 1);
			SampleColor.rgb /= 1 + Luminance(SampleColor.rgb);
			outColor += SampleColor;
			NumSamples++;
		}
	}
	outColor /= NumSamples;
	outColor.rgb /= 1 - Luminance(outColor.rgb);
	outColor *= GetRoughnessFade();
	return outColor;
}

void	ApplyTechnique() {
	Out.Color = SSR();
}

)""