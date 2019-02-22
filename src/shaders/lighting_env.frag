R""(
#define	KERNEL_SIZE				9

const vec2 poissonDisk[] = vec2[KERNEL_SIZE](
	vec2(0.95581, -0.18159), vec2(0.50147, -0.35807), vec2(0.69607, 0.35559),
	vec2(-0.0036825, -0.59150),	vec2(0.15930, 0.089750), vec2(-0.65031, 0.058189),
	vec2(0.11915, 0.78449),	vec2(-0.34296, 0.51575), vec2(-0.60380, -0.41527));

uniform vec3		brightnessDotValue = vec3(0.299, 0.587, 0.114);
uniform vec3		envGammaCorrection = vec3(2.2);
uniform sampler2D	LastColor;
uniform sampler2D	LastEmitting;
uniform sampler2D	LastDepth;

vec3	UVFromPosition(in vec3 position)
{
	vec4	projectedPosition = Camera.Matrix.Projection * Camera.Matrix.View * vec4(position, 1);
	projectedPosition /= projectedPosition.w;
	projectedPosition = projectedPosition * 0.5 + 0.5;
	return (projectedPosition.xyz);
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
	ret.xyz += sampleLod(LastEmitting, sampleUV.xy, Frag.Material.Roughness).rgb * UVSamplingAttenuation.x; //LastEmitting is already blurred
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

/* vec3 rotateVec3()
{
	vec3 A, B;

	vec3 R; // rotation axis (normalized)

	vec3 A_r   = R * dot(A, R); // component of A, in the direction of R
	vec3 A_prj = A - A_r;       // component of A, in the rotation plane

	vec3 B_r   = R * dot(B, R); // component of B, in the direction of R
	vec3 B_prj = B - B_r;       // component of B, in the rotation plane

	vec3 AB    = length(A_prj) * normalize(B_prj) + A_r;
}
 */
vec4	SSR()
{
	vec3	V = normalize(Frag.Position - Camera.Position);
	vec3	R = reflect(V, Frag.Normal);
	int		sampleNbr = REFLEXION_SAMPLES + 1;
	vec3	RSDirs[REFLEXION_SAMPLES + 1];

	RSDirs[0] = R;
	for (uint i = 0; i < sampleNbr; i++) {
		vec2 offset = poissonDisk[i % 9] * (0.1 * Frag.Material.Roughness + 0.0001);
		offset = rotateUV(offset, randomAngle(Frag.Position, 1024), vec2(0));
		vec3	n = DirectionFromVec2(offset);
		RSDirs[i] = reflect(V, n);
		/* RSDirs[i] = R + vec3(offset.x, offset.y, offset.x * offset.y);
		RSDirs[i] = normalize(RSDirs[i]); */
	}
	float	curLength = 0.5;
	vec4	ret = vec4(0);
	float	hits = 0;
	//float CameraFacingReflectionAttenuation = 1 - smoothstep(0.25, 0.5, dot(-V, R));
	//if (CameraFacingReflectionAttenuation <= 0)
	//	return (vec4(0));

	for (uint i = 0; i < REFLEXION_STEPS; i++)
	{
		vec3	curPos = R * curLength + Frag.Position; //Calculate current step's position
		vec3	curUV = UVFromPosition(curPos); //Compute step's screen coordinates
		float	curDepth = texture(LastDepth, curUV.xy).r;
		float	sampleAngle = randomAngle(curPos, 1024);
		if (curUV.z > 1)
			break;
		for (uint j = 0; j < sampleNbr; j++)
		{
			vec3	sampleUV = UVFromPosition(RSDirs[j] * curLength + Frag.Position);
			//sampleUV.xy = rotateUV(sampleUV.xy, sampleAngle, curUV.xy);
			float	sampleDepth = texture(LastDepth, sampleUV.xy).r;
			if (sampleDepth < 1 && sampleUV.z > sampleDepth && abs(sampleUV.z - sampleDepth) <= 0.0025)
			{
				float	screenEdgeFactor = 1;
				screenEdgeFactor -= smoothstep(0, 1, pow(abs(sampleUV.x * 2 - 1), SCREEN_BORDER_FACTOR)); //Attenuate reflection factor when getting closer to screen border
				screenEdgeFactor -= smoothstep(0, 1, pow(abs(sampleUV.y * 2 - 1), SCREEN_BORDER_FACTOR));
				screenEdgeFactor *= max(0, dot(RSDirs[j], V)) * max(0, dot(RSDirs[j], R));
				if (screenEdgeFactor > 0)
				{
					hits++;
					screenEdgeFactor = clamp(screenEdgeFactor, 0, 1);
					ret.xyz += sampleLod(LastColor, sampleUV.xy, Frag.Material.Roughness * 2).rgb * screenEdgeFactor; //Sample last image color and accumulate it
					ret.xyz += sampleLod(LastEmitting, sampleUV.xy, Frag.Material.Roughness).rgb * screenEdgeFactor; //LastEmitting is already blurred
					ret.w += screenEdgeFactor;
				}
			}
		}
		curLength = length(Frag.Position - Position(curUV.xy, curDepth)); //Advance in ray marching proportionaly to current point's distance (make sure you don't miss anything)
		//curLength += curLength;
		//curLength = length(Frag.Position - Position(curUV.xy, sampleDepth)); //Advance in ray marching proportionaly to current point's distance (make sure you don't miss anything)
	}
	if (hits > 0) {
		ret /= hits; //Compute average color and attenuation
		//ret.w *= CameraFacingReflectionAttenuation;
	}
	return (ret);
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
	if (ssrResult.w > 0) {
		Out.Color.rgb += mix(envReflection, ssrResult.xyz * fresnel, clamp(ssrResult.w, 0, 1));
	}
	else {
		Out.Color.rgb += envReflection;
	}
	Out.Color.rgb += (diffuse + Frag.Material.Emitting) * alpha;
	Out.Color.a = 1;
	Out.Emitting.rgb += max(vec3(0), Out.Color.rgb - 1) + Frag.Material.Emitting;
}

)""