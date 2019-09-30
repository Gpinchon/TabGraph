R""(
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

int ditherMatrix[] = int[16](
	0,	8,	2,	10,
	12,	4,	14,	6,
	3,	11,	1,	9,
	15,	7,	13,	5
);

int dither(ivec2 position) {
  int x = position.x % 4;
  int y = position.y % 4;
  int index = x + y * 4;
  return ditherMatrix[index % 16];
}

#define CELL_STEP_OFFSET 0.05
/* 
void StepThroughCell(inout vec3 RaySample, vec3 RayDir, int MipLevel)
{
	ivec2	MipSize = textureSize(LastDepth, MipLevel);
	vec2	CellSize = 1 / vec2(MipSize);
	RaySample += RayDir * (CellSize.x * CellSize.y);
} */

void StepThroughCell(inout vec3 RaySample, vec3 RayDir, int MipLevel)
{
	ivec2 MipSize = textureSize(LastDepth, MipLevel);
	vec2 MipCellIndex = RaySample.xy * vec2(MipSize);
	vec2 BoundaryUV;
	BoundaryUV.x = RayDir.x > 0 ?
		ceil(MipCellIndex.x) / float(MipSize.x) : 
		floor(MipCellIndex.x) / float(MipSize.x);
	BoundaryUV.y = RayDir.y > 0 ?
		ceil(MipCellIndex.y) / float(MipSize.y) : 
		floor(MipCellIndex.y) / float(MipSize.y);
	vec2 t;
	t.x = (BoundaryUV.x - RaySample.x) / RayDir.x;
	t.y = (BoundaryUV.y - RaySample.y) / RayDir.y;
	if (abs(t.x) < abs(t.y))
	{
		RaySample += (t.x + CELL_STEP_OFFSET / float(MipSize.x)) * RayDir;
	}
	else
	{
		RaySample += (t.y + CELL_STEP_OFFSET / float(MipSize.y)) * RayDir;
	}
}

float cross(in vec2 a, in vec2 b)
{
    return a.x * b.y - b.x * a.y;
}

bool lineSegmentIntersection(out float intersection, in vec2 o, in vec2 d, in vec2 a, in vec2 b)
{
	vec2	v1 = o - a;
	vec2	v2 = b - a;
	vec2	v3 = vec2(-d.y, d.x);
	float	t1 = cross(v2, v1) / dot(v2, v3);
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

bool	castRay(inout vec3 RayOrigin, in vec3 RayDir, in float stepOffset, out int MipLevel)
{
	float	distanceToBorder = 1.0;
	for (int side = 0; side < 4; side++)
	{
		if (lineSegmentIntersection(distanceToBorder, RayOrigin.xy, RayDir.xy,
					vec2(sides[side].x, sides[side].y), vec2(sides[side].z, sides[side].w)))
			break;
	}
	int		minMipMaps = 0;
	int		maxMipMaps = textureMaxLod(LastDepth);
	int		maxTries = 64;
	float	step = distanceToBorder / float(maxTries);
	float 	compareTolerance = abs(RayOrigin.z) * step * 2;
	float	sampleDist = stepOffset * step + step;
	MipLevel = minMipMaps;
	for (int tries = 0; tries < maxTries; tries++)
	{
		vec3	curUV = RayOrigin + RayDir * sampleDist;
		
		//MipLevel = int(Frag.Material.Roughness * (tries * 4.0 / float(maxTries)) + minMipMaps);
		float	sampleDepth = texelFetchLod(LastDepth, curUV.xy, MipLevel).r;
		float	depthDiff = curUV.z - sampleDepth;
		bool	hit = abs(depthDiff) < compareTolerance;
		if (hit && sampleDepth < curUV.z) {
			RayOrigin = curUV;
			return curUV.z < 1;
		}
		sampleDist += step;
		MipLevel += int(mix(minMipMaps, maxMipMaps, Frag.Material.Roughness * (sampleDist / distanceToBorder)));
	}
}

/* bool	castRay(inout vec3 RayOrigin, in vec3 RayDir, in float stepOffset, out int MipLevel)
{
	float	distanceToBorder = 1.0;
	vec2	rayDir2D = normalize(RayOrigin.xy - (RayOrigin + RayDir).xy);
	for (int side = 0; side < 4; side++)
	{
		lineSegmentIntersection(distanceToBorder, RayOrigin.xy, rayDir2D,
			vec2(sides[side].x, sides[side].y), vec2(sides[side].z, sides[side].w));
	}
	int		maxTries = 64;
	int		maxMipMaps = textureMaxLod(LastDepth);
	int		minMipMaps = 0;
	MipLevel = minMipMaps;
	float	step = distanceToBorder / float(maxTries);
	float 	compareTolerance = abs(RayOrigin.z) * step * 2;
	float	minHit = 1;
	float	lastDepthDiff = 0;
	float	sampleDist = stepOffset * step + step + 0.1f;
	for (int tries = 0; tries < maxTries; tries++)
	{
		vec3	curUV = RayOrigin + RayDir * sampleDist;
		MipLevel = int(Frag.Material.Roughness * (tries * 4.0 / float(maxTries)) + minMipMaps);
		float	sampleDepth = texelFetchLod(LastDepth, curUV.xy, MipLevel).r;
		float	depthDiff = curUV.z - sampleDepth;
		bool	hit = abs(depthDiff + compareTolerance) < compareTolerance;
		float	depthLerp = clamp(0, 1, lastDepthDiff / (lastDepthDiff - depthDiff));
		float	intersectDist = sampleDist + depthLerp * step - step;
		float	hitDist = hit ? intersectDist : 1;
		minHit = min(minHit, hitDist);
		sampleDist += step;
		lastDepthDiff = depthDiff;
	}
	RayOrigin += RayDir * minHit;
	return minHit < 1;
} */

/* bool	castRay(inout vec3 RayOrigin, in vec3 RayDir)
{
	if (length(RayDir) < 0.01)
		return false;

	int		maxTries = 256;
	int		maxMipMaps = textureMaxLod(LastDepth);
	int		minMipMaps = 0;
	int		mipLevel = minMipMaps;
	int		tries = 0;
	vec3	curUV = RayOrigin;
	float 	compareTolerance = 0.001;
	StepThroughCell(curUV, RayDir, mipLevel);
	while (mipLevel >= minMipMaps && mipLevel < maxMipMaps - 1 && tries < maxTries)
	{
		if (curUV.z >= 1 || curUV.z <= 0 || any(lessThan(curUV.xy, vec2(0))) || any(greaterThan(curUV.xy, vec2(1)))) {
			break;
		}
		float	sampleDepth = texelFetchLod(LastDepth, curUV.xy, mipLevel).r;
		float	DepthDiff = curUV.z - sampleDepth;
		bool Hit = abs(DepthDiff + compareTolerance) < compareTolerance;
		if (Hit) //We intersected
		{
			mipLevel--;
		}
		else
		{
			mipLevel++;
			tries++;
			StepThroughCell(curUV, RayDir, mipLevel);
		}
		
	} 
	RayOrigin = curUV;
	return mipLevel <= minMipMaps;
} */

float PseudoRandom(vec2 xy)
{
	vec2 pos = fract(xy / 128.0f) * 128.0f + vec2(-64.340622f, -72.465622f);
	return fract(dot(pos.xyx * pos.xyy, vec3(20.390625f, 60.703125f, 2.4281209f)));
}

vec4	SSR()
{
	float	Depth = texelFetchLod(LastDepth, Frag.UV, 0).r;
	vec3	WSPos = Position(Frag.UV, Depth);
	vec3	WSViewDir = normalize(WSPos - Camera.Position);
	vec3	WSReflectionDir = reflect(WSViewDir, Frag.Normal);
	vec3	SSPos = vec3(Frag.UV, Depth);
	vec3	SSPoint = UVFromPosition(10 * WSReflectionDir + WSPos);
	vec3	SSRDir = normalize(SSPoint - SSPos);
	float	SSROffset = PseudoRandom(vec2(textureSize(LastDepth, 0) * Frag.UV));
	int		MipLevel;
	if (castRay(SSPos, SSRDir, SSROffset, MipLevel))
	{
		//float SSRParticipation = 1;
		//float SSRParticipation = max(0, dot(WSViewDir, WSReflectionDir));
		float SSRParticipation = 1 - smoothstep(0.25, 0.50, dot(-WSViewDir, WSReflectionDir));
		SSRParticipation *= smoothstep(-0.17, 0.0, dot(texture(LastNormal, SSPos.xy, 0).xyz, -WSReflectionDir));
		SSRParticipation -= smoothstep(0, 1, pow(abs(SSPos.x * 2 - 1), SCREEN_BORDER_FACTOR)); //Attenuate reflection factor when getting closer to screen border
		SSRParticipation -= smoothstep(0, 1, pow(abs(SSPos.y * 2 - 1), SCREEN_BORDER_FACTOR));
		SSRParticipation = clamp(SSRParticipation, 0, 1);
		int		maxMipMaps = textureMaxLod(LastDepth);
		return vec4(sampleLod(LastColor, SSPos.xy, (MipLevel / maxMipMaps) /* * Frag.Material.Roughness * 2 */).rgb * SSRParticipation, SSRParticipation);
	}
	return vec4(0);
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
