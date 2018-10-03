uniform sampler2D	opaqueBackColor;
uniform sampler2D	opaqueBackEmitting;

float	cmix(float min, float max, float percent)
{
	if (percent < 0 || percent > 1)
		return (mix(min, max, 1 - mod(percent, 1)));
	return (mix(min, max, percent));
}

vec2	cmix(vec2 min, vec2 max, vec2 percent)
{
	return (vec2(cmix(min.x, max.x, percent.x), cmix(min.y, max.y, percent.y)));
}

float	warpUV(float min, float max, float percent)
{
	if (percent < 0 || percent > 1)
		return (smoothstep(min, max, 1 - mod(percent, 1)));
	return (percent);
}

vec2	warpUV(vec2 min, vec2 max, vec2 percent)
{
	return (vec2(warpUV(min.x, max.x, percent.x), warpUV(min.y, max.y, percent.y)));
}

vec3	Fresnel(in float factor, in vec3 F0, in float roughness)
{
	return ((max(vec3(1 - roughness), F0)) * pow(max(0, 1 - factor), 5) + F0);
}

uniform float	step = 0.1;
uniform float	minRayStep = 0.1;
uniform int		maxSteps = 30;
uniform float	searchDist = 5;
uniform int		numBinarySearchSteps = 5;
uniform float	reflectionSpecularFalloffExponent = 3.0;

vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth)
{
	float depth;

	vec4 projectedCoord;
	for(int i = 0; i < numBinarySearchSteps; i++)
	{
		projectedCoord = Camera.Matrix.Projection * vec4(hitCoord, 1.0);
		projectedCoord /= projectedCoord.w;
		projectedCoord = projectedCoord * 0.5 + 0.5;
		depth = texture(Texture.Depth, projectedCoord.xy).r;
		if (depth == 1)
			continue;
		dDepth = projectedCoord.z - depth;
		dir *= 0.5;
		if(dDepth > 0.0) {
			hitCoord += dir;
		}
		else {
			hitCoord -= dir;
		}
	}

	projectedCoord = Camera.Matrix.Projection * vec4(hitCoord, 1.0);
	projectedCoord.xy /= projectedCoord.w;
	projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

	return vec3(projectedCoord.xy, depth);
}

void	SSR()
{
	vec3	V = normalize(Camera.Position - Frag.Position);
	vec3	reflectDir = -normalize(mat3(Camera.Matrix.View) * normalize(reflect(V, Frag.Normal)));
	vec4	hitPos = Camera.Matrix.View * vec4(Frag.Position, 1);
	float	dDepth;
	vec4	projectedCoord;
	vec3	result = vec3(0);

	reflectDir *= step;
	for (int i = 0; i < maxSteps && reflectDir.z < 0; i++)
	{
		hitPos.xyz += reflectDir;
		projectedCoord = Camera.Matrix.Projection * hitPos;
		projectedCoord /= projectedCoord.w;
		projectedCoord = projectedCoord * 0.5 + 0.5;
		float sampleDepth = texture(Texture.Depth, projectedCoord.xy).r;
		if (sampleDepth == 1)
			continue;
		dDepth = projectedCoord.z - sampleDepth;
		if (dDepth <= 0)
		{
			//hitPos.xyz = texture(Texture.Albedo, projectedCoord.xy).rgb;
			//reflectDir = -reflectDir;
			//result = BinarySearch(reflectDir, hitPos.xyz, dDepth);
			result = projectedCoord.xyz;
			break;
		}
	}
	Out.Color.rgb = texture(opaqueBackColor, result.xy).rgb;
	//Out.Color.rgb = hitPos.xyz;
	/* vec3	viewNormal	= mat3(Camera.Matrix.View) * Frag.Normal;
	vec3	V = normalize(Camera.Position - Frag.Position);
	vec3	viewPos		= mat3(Camera.Matrix.View) * Frag.Position;
	vec3	reflectDir	= normalize(reflect(normalize(viewPos), normalize(viewNormal)));
	vec3	hitPos		= viewPos;
	float	dDepth;
	RayCastSSR(reflectDir, hitPos, dDepth);
	Out.Color.rgb = viewPos; */

	//Out.Color.rgb = viewNormal;
}

void	ApplyTechnique()
{
	vec2	refract_UV = Frag.UV;

	if (Frag.Material.Ior > 1)
	{
		vec3	V = normalize(Camera.Position - Frag.Position);
		float	NdV = dot(Frag.Normal, V);
		if (NdV < 0) {
			Frag.Normal = -Frag.Normal;
			NdV = -NdV;
		}
		vec3	fresnel = Fresnel(NdV, Frag.Material.Specular, Frag.Material.Roughness);
		vec2	refractFactor = vec2(1 - Frag.Depth) * vec2(0.25f) + (fresnel.x + fresnel.y + fresnel.z) / 3.f * 0.0125f;
		vec2	refractDir = (mat3(Camera.Matrix.View) * normalize(refract(V, Frag.Normal, 1.0 / Frag.Material.Ior))).xy;
		refract_UV = refractDir * refractFactor + Frag.UV;
		refract_UV = warpUV(vec2(0), vec2(1), refract_UV);
	}
	vec3	Back_Color = sampleLod(opaqueBackColor, refract_UV, Frag.Material.Roughness).rgb;
	vec3	Back_Emitting = sampleLod(opaqueBackEmitting, refract_UV, Frag.Material.Roughness).rgb;
	if (Out.Color.a == 0) {
		Out.Color = vec4(Back_Color, 1);
		Out.Emitting = Back_Emitting;
		return ;
	}
	Back_Color = mix(Back_Color, Back_Color * Frag.Material.Albedo, Frag.Material.Alpha);
	Back_Emitting = mix(Back_Emitting, Back_Emitting * Frag.Material.Albedo, Frag.Material.Alpha);
	Out.Color.rgb += Back_Color * (1 - Frag.Material.Alpha);
	Out.Color.a = 1;
	Out.Emitting.rgb += Back_Emitting * (1 - Frag.Material.Alpha);
	SSR();
}