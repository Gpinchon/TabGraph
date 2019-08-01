R""(
#define	KERNEL_SIZE				64

vec2 poissonDisk[] = vec2[KERNEL_SIZE] (
	vec2(-0.613392, 0.617481),
	vec2(0.170019, -0.040254),
	vec2(-0.299417, 0.791925),
	vec2(0.645680, 0.493210),
	vec2(-0.651784, 0.717887),
	vec2(0.421003, 0.027070),
	vec2(-0.817194, -0.271096),
	vec2(-0.705374, -0.668203),
	vec2(0.977050, -0.108615),
	vec2(0.063326, 0.142369),
	vec2(0.203528, 0.214331),
	vec2(-0.667531, 0.326090),
	vec2(-0.098422, -0.295755),
	vec2(-0.885922, 0.215369),
	vec2(0.566637, 0.605213),
	vec2(0.039766, -0.396100),
	vec2(0.751946, 0.453352),
	vec2(0.078707, -0.715323),
	vec2(-0.075838, -0.529344),
	vec2(0.724479, -0.580798),
	vec2(0.222999, -0.215125),
	vec2(-0.467574, -0.405438),
	vec2(-0.248268, -0.814753),
	vec2(0.354411, -0.887570),
	vec2(0.175817, 0.382366),
	vec2(0.487472, -0.063082),
	vec2(-0.084078, 0.898312),
	vec2(0.488876, -0.783441),
	vec2(0.470016, 0.217933),
	vec2(-0.696890, -0.549791),
	vec2(-0.149693, 0.605762),
	vec2(0.034211, 0.979980),
	vec2(0.503098, -0.308878),
	vec2(-0.016205, -0.872921),
	vec2(0.385784, -0.393902),
	vec2(-0.146886, -0.859249),
	vec2(0.643361, 0.164098),
	vec2(0.634388, -0.049471),
	vec2(-0.688894, 0.007843),
	vec2(0.464034, -0.188818),
	vec2(-0.440840, 0.137486),
	vec2(0.364483, 0.511704),
	vec2(0.034028, 0.325968),
	vec2(0.099094, -0.308023),
	vec2(0.693960, -0.366253),
	vec2(0.678884, -0.204688),
	vec2(0.001801, 0.780328),
	vec2(0.145177, -0.898984),
	vec2(0.062655, -0.611866),
	vec2(0.315226, -0.604297),
	vec2(-0.780145, 0.486251),
	vec2(-0.371868, 0.882138),
	vec2(0.200476, 0.494430),
	vec2(-0.494552, -0.711051),
	vec2(0.612476, 0.705252),
	vec2(-0.578845, -0.768792),
	vec2(-0.772454, -0.090976),
	vec2(0.504440, 0.372295),
	vec2(0.155736, 0.065157),
	vec2(0.391522, 0.849605),
	vec2(-0.620106, -0.328104),
	vec2(0.789239, -0.419965),
	vec2(-0.545396, 0.538133),
	vec2(-0.178564, -0.596057));

uniform vec3		brightnessDotValue = vec3(0.299, 0.587, 0.114);
uniform vec3		envGammaCorrection = vec3(2.2);
uniform sampler2D	LastColor;
uniform sampler2D	LastNormal;
uniform sampler2D	LastDepth;

vec3	UVFromPosition(in vec3 position)
{
	vec4	projectedPosition = Camera.Matrix.Projection * Camera.Matrix.View * vec4(position, 1);
	projectedPosition /= projectedPosition.w;
	projectedPosition = projectedPosition * 0.5 + 0.5;
	return projectedPosition.xyz;
}

vec3	DirectionFromVec2(in vec2 perturbation)
{
	mat3	tbn = tbn_matrix();
	vec3	new_normal = vec3(perturbation, 1) * tbn;
	return normalize(new_normal);
}

/*
vec2	UVSamplingAttenuation = smoothstep(vec2(0.05), vec2(0.1), sampleUV.xy) * (1 - smoothstep(vec2(0.95),vec2 (1), sampleUV.xy));
UVSamplingAttenuation.x *= UVSamplingAttenuation.y;
if (UVSamplingAttenuation.x > 0)
{
	UVSamplingAttenuation.x = clamp(UVSamplingAttenuation.x, 0, 1);
	ret.xyz += sampleLod(LastColor, sampleUV.xy, Frag.Material.Roughness * 2).rgb * UVSamplingAttenuation.x; //Sample last image color and accumulate it
	ret.xyz += sampleLod(LastNormal, sampleUV.xy, Frag.Material.Roughness).rgb * UVSamplingAttenuation.x; //LastNormal is already blurred
	ret.w += UVSamplingAttenuation.x;
}
*/

vec2 rotateUV(vec2 uv, float rotation, vec2 mid)
{
	return vec2(
		cos(rotation) * (uv.x - mid.x) + sin(rotation) * (uv.y - mid.y) + mid.x,
		cos(rotation) * (uv.y - mid.y) - sin(rotation) * (uv.x - mid.x) + mid.y
		);
}

float dither(ivec2 position) {
  int x = position.x % 4;
  int y = position.y % 4;
  int index = x + y * 4;
  float value = 0.0;
  if (x < 8) {
    if (index == 0) value = 0.00;
    else if (index == 1) value = 0.08;
    else if (index == 2) value = 0.02;
    else if (index == 3) value = 0.10;
    else if (index == 4) value = 0.12;
    else if (index == 5) value = 0.04;
    else if (index == 6) value = 0.14;
    else if (index == 7) value = 0.06;
    else if (index == 8) value = 0.03;
    else if (index == 9) value = 0.11;
    else if (index == 10) value = 0.01;
    else if (index == 11) value = 0.09;
    else if (index == 12) value = 0.15;
    else if (index == 13) value = 0.07;
    else if (index == 14) value = 0.13;
    else if (index == 15) value = 0.05;
  }
  return value;
}

float dither(ivec2 position, float brightness) {
  int x = position.x % 4;
  int y = position.y % 4;
  int index = x + y * 4;
  float limit = 0.0;

  if (x < 8) {
    if (index == 0) limit = 0.0625;
    else if (index == 1) limit = 0.5625;
    else if (index == 2) limit = 0.1875;
    else if (index == 3) limit = 0.6875;
    else if (index == 4) limit = 0.8125;
    else if (index == 5) limit = 0.3125;
    else if (index == 6) limit = 0.9375;
    else if (index == 7) limit = 0.4375;
    else if (index == 8) limit = 0.25;
    else if (index == 9) limit = 0.75;
    else if (index == 10) limit = 0.125;
    else if (index == 11) limit = 0.625;
    else if (index == 12) limit = 1.0;
    else if (index == 13) limit = 0.5;
    else if (index == 14) limit = 0.875;
    else if (index == 15) limit = 0.375;
  }
  return brightness < limit ? 0.0 : 1.0;
}

#define CELL_STEP_OFFSET 0.05

void StepThroughCell(inout vec3 RaySample, vec3 RayDir, int MipLevel)
{
	ivec2 MipSize = textureSize(LastDepth, MipLevel);
	vec2 MipCellIndex = RaySample.xy * vec2(MipSize);
	vec2 BoundaryUV;
	BoundaryUV.x = RayDir.x > 0 ? ceil(MipCellIndex.x) / float(MipSize.x) : 
		floor(MipCellIndex.x) / float(MipSize.x);
	BoundaryUV.y = RayDir.y > 0 ? ceil(MipCellIndex.y) / float(MipSize.y) : 
		floor(MipCellIndex.y) / float(MipSize.y);
	vec2 t;
	t.x = (BoundaryUV.x - RaySample.x) / RayDir.x;
	t.y = (BoundaryUV.y - RaySample.y) / RayDir.y;
	if (abs(t.x) < abs(t.y))
	{
		RaySample += (t.x + CELL_STEP_OFFSET / MipSize.x) * RayDir;
	}
	else
	{
		RaySample += (t.y + CELL_STEP_OFFSET / MipSize.y) * RayDir;
	}
}

/* bool	castRay(inout vec3 rayOrigin, in vec3 rayDir)
{
	bool intersects = false;
	for (int RayStep = 0; RayStep < REFLEXION_STEPS; RayStep++)
	{
		vec3 RaySample = (RayStep * 0.02) * rayDir + rayOrigin;
		float Depth = texture(LastDepth, RaySample.xy, 0).r;
		if (RaySample.z > Depth )
		{
			intersects = true;
			rayOrigin = RaySample;
			break;
		}
	}
	return intersects;
} */

bool	castRay(inout vec3 rayOrigin, in vec3 rayDir)
{
	int		mipLevel = 0;
	int		maxMipMaps = textureMaxLod(LastDepth);
	int		tries = 0;
	vec3	curUV = rayOrigin;
	bool	intersects = false;
	StepThroughCell(curUV, rayDir, mipLevel);
	while (mipLevel >= 0 && mipLevel < maxMipMaps - 1)// && tries < 1024)
	{
		if (!intersects) StepThroughCell(curUV, rayDir, mipLevel);
		vec2 UVSamplingAttenuation = smoothstep(0.05, 0.1, curUV.xy) * (1.f-smoothstep(0.95, 1.0, curUV.xy));
		if (curUV.z > 1 || curUV.z < 0 || any(lessThan(curUV.xy, vec2(0))) || any(greaterThan(curUV.xy, vec2(1)))) {
		//if (any(equal(UVSamplingAttenuation, vec2(0)))) {
			intersects = false;
			break;
		}
		float	sampleDepth = texture(LastDepth, curUV.xy, mipLevel).r;
		if (curUV.z > sampleDepth) //We intersected
		{
			//float t = (curUV.z - sampleDepth) / rayDir.z;
			//curUV -= rayDir * t;
			rayOrigin = curUV;
			intersects = true;
			mipLevel--;
		}
		else
		{
			intersects = false;
			mipLevel++;
			tries++;
		}
	}
	return mipLevel < 0;
}

vec4	SSR()
{
	float	Depth = texture(LastDepth, Frag.UV, 0).r;
	vec3	WSPos = Position(Frag.UV, Depth);
	vec3	WSViewDir = normalize(WSPos - Camera.Position);
	vec3	WSReflectionDir = reflect(WSViewDir, Frag.Normal);
	vec3	SSPos = vec3(Frag.UV, Depth);
	vec3	SSPoint = UVFromPosition(10 * WSReflectionDir + WSPos);
	vec3	SSRDir = normalize(SSPoint - SSPos);
	float	SSRDither = dither(ivec2(textureSize(LastDepth, 0) * Frag.UV));
	SSPos += (SSRDither + 0.001) * SSRDir;
	float SSRParticipation = 1 - smoothstep(0.25, 0.5, dot(-WSViewDir, WSReflectionDir));
	if (SSRParticipation > 0 && castRay(SSPos, SSRDir))
	{
		SSRParticipation *= smoothstep(-0.17, 0.0, dot(texture(LastNormal, SSPos.xy, 0).xyz, -WSReflectionDir));
		SSRParticipation -= smoothstep(0, 1, pow(abs(SSPos.x * 2 - 1), SCREEN_BORDER_FACTOR)); //Attenuate reflection factor when getting closer to screen border
		SSRParticipation -= smoothstep(0, 1, pow(abs(SSPos.y * 2 - 1), SCREEN_BORDER_FACTOR));
		//return vec4(SSRDir, 1);
		return vec4(sampleLod(LastColor, SSPos.xy, Frag.Material.Roughness * 2).rgb, SSRParticipation);
	}
	return vec4(0);
	
	/*int		sampleNbr = min(63, REFLEXION_SAMPLES + 1);
	vec3	SSRDirs[REFLEXION_SAMPLES + 1];
	SSRDirs[0] = R;
	float	offsetRotation = randomAngle(Frag.UV, 1024);
	float	ditherFactor = dither(ivec2(textureSize(LastDepth, 0) * Frag.UV), (Frag.Material.Roughness * Frag.Material.Roughness + 0.0001));
	for (uint i = 0; i < sampleNbr; i++) {
		vec2 offset = poissonDisk[i % KERNEL_SIZE];
		offset *= ditherFactor;
		//offset *= (Frag.Material.Roughness * Frag.Material.Roughness + 0.0001);
		offset = rotateUV(offset, offsetRotation, vec2(0));
		SSRDirs[i] = reflect(WSViewDir, DirectionFromVec2(offset));
	}
	vec4	ret = vec4(0);
	float	hits = 0;
	for (uint j = 0; j < sampleNbr; j++)
	{
		vec2	intersectionUV;
		bool	intersected = castRay(WSPos, SSRDirs[j], intersectionUV);
		if (!intersected)
			continue;
		
		//return vec4(ReflectionNormalColor, 1);
		//float DirectionBasedAttenuation = smoothstep(-0.17, 0.0, dot(RSNormal.xyz, -SSRDirs[j]));
		
		vec3 RSNormal = texture(LastNormal, intersectionUV).xyz;
		float	RSParticipation = 1;
		RSParticipation -= smoothstep(0, 1, pow(abs(intersectionUV.x * 2 - 1), SCREEN_BORDER_FACTOR)); //Attenuate reflection factor when getting closer to screen border
		RSParticipation -= smoothstep(0, 1, pow(abs(intersectionUV.y * 2 - 1), SCREEN_BORDER_FACTOR));
		RSParticipation = max(0, RSParticipation);
		//RSParticipation *= dot(WSViewDir, SSRDirs[j]);
		RSParticipation *= 1 - smoothstep(0.25, 0.5, dot(-WSViewDir, SSRDirs[j]));
		RSParticipation *= smoothstep(-0.17, 0.0, dot(RSNormal.xyz, -SSRDirs[j]));//step(0.17, -dot(RSNormal.xyz, SSRDirs[j]));
		//RSParticipation *= max(0, -dot(RSNormal.xyz, SSRDirs[j]));
		//RSParticipation *= smoothstep(0.0, 1.0, max(0, 1 - dot(RSNormal.xyz, SSRDirs[j])));
		if (RSParticipation <= 0)
			continue;
		ret.xyz += sampleLod(LastColor, intersectionUV.xy, Frag.Material.Roughness * 2).rgb * RSParticipation;
		ret.w += RSParticipation;
		hits++;
	}
	if (hits > 0) {
		ret /= hits; //Compute average color and attenuation
	}
	return (ret);*/
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
		brightness = dot(pow(Out.Color.rgb, envGammaCorrection), brightnessDotValue);
		Out.Emitting = max(vec3(0), (Out.Color.rgb - 0.8) * min(1, brightness));
		return ;
		#endif //TRANSPARENT
	}
	vec3	fresnel = min(vec3(0.5), Fresnel(NdV, Frag.Material.Specular, Frag.Material.Roughness));
	reflection *= fresnel;
	brightness = dot(pow(reflection_spec, envGammaCorrection), brightnessDotValue);
	reflection_spec *= brightness * min(fresnel + 0.5, fresnel * Env_Specular(NdV, Frag.Material.Roughness));
	specular *= fresnel * brdf.x + mix(vec3(1), fresnel, Frag.Material.Metallic) * brdf.y;
	diffuse *= Frag.Material.Albedo.rgb * (1 - Frag.Material.Metallic);

	float	alpha = Frag.Material.Alpha + max(specular.r, max(specular.g, specular.b));
	alpha = min(1, alpha);

	vec3	envReflection = (specular + reflection_spec + reflection) * alpha;

	vec4	ssrResult = vec4(0);
	if (Frag.Material.Roughness < 1) {
		ssrResult = SSR();
	}
	Out.Color.rgb += mix(envReflection, ssrResult.xyz * fresnel, ssrResult.w);
	Out.Color.rgb += (diffuse + Frag.Material.Emitting) * alpha;
	Out.Color.a = 1;
	Out.Emitting.rgb += max(vec3(0), Out.Color.rgb - 1) + Frag.Material.Emitting;
}

)""