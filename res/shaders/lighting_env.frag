uniform vec3	brightnessDotValue = vec3(0.299, 0.587, 0.114); //For optimization, not meant to be set
uniform vec3	envGammaCorrection = vec3(2.2); //For optimization, not meant to be set

uniform sampler2D	LastColor;
uniform sampler2D	LastEmitting;
uniform sampler2D	LastDepth;

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
		if (projectedCoord.x > 1 || projectedCoord.x < 0 || projectedCoord.y > 1 || projectedCoord.y < 0)
			continue;
		depth = sampleLod(Texture.Depth, projectedCoord.xy, 0.1).r;
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

bool	LineSegmentsIntersect(in vec2 p, in vec2 p2, in vec2 q, in vec2 q2, out vec2 intersection);
bool	LineSegmentsIntersect(in vec3 a0, in vec3 a1, in vec3 b0, in vec3 b1, out vec3 ip);
bool	LineSegmentsIntersect(in vec3 p1, in vec3 p2, in vec3 p3, in vec3 p4, out vec3 pa, out vec3 pb,
   out float mua, out float mub);

vec2	screenSides[] = vec2[4](
	vec2(-1, 1), vec2(1, 1),
	vec2(1, -1), vec2(-1, -1)
);

vec3	UVFromPosition(in vec3 position)
{
	vec4	projectedPosition = Camera.Matrix.Projection * Camera.Matrix.View * vec4(position, 1);
	projectedPosition /= projectedPosition.w;
	projectedPosition = projectedPosition * 0.5 + 0.5;
	return (projectedPosition.xyz);
}

#define	KERNEL_SIZE 9

uniform vec2 poissonDisk[] = vec2[KERNEL_SIZE](
	vec2(0.95581, -0.18159), vec2(0.50147, -0.35807), vec2(0.69607, 0.35559),
	vec2(-0.0036825, -0.59150),	vec2(0.15930, 0.089750), vec2(-0.65031, 0.058189),
	vec2(0.11915, 0.78449),	vec2(-0.34296, 0.51575), vec2(-0.60380, -0.41527));

vec4	SSR()
{
	/* float	dDepth;
	vec3	V = normalize(Frag.Position - Camera.Position);
	vec3	reflectDir = reflect(V, Frag.Normal);
	reflectDir = mat3(Camera.Matrix.View) * reflectDir;
	vec3	startPos = (Camera.Matrix.View * vec4(Frag.Position, 1)).xyz;
	vec4	projectedCoord;
	vec4	ret = vec4(0, 0, 1, -reflectDir.z);
	vec3	endPos; */

	//return (vec4(atan(reflectDir.xy), 0, 0));
	/* for (int i = 0; i < 4; i++)
	{
		vec2	side0 = screenSides[i];
		vec2	side1 = screenSides[(i + 1) % 4];
		vec2	c = Frag.UV * 2 - 1;
		vec2	d = c + 100 * normalize(reflectDir.xy);
		vec2	intersection;
		if (LineSegmentsIntersect(side0, side1, c, d, intersection)) {
			endPos = startPos + reflectDir * distance(c, intersection);
			return (vec4(distance(c, intersection) / 2.828427));
			// vec3	a0 = vec3(intersection, 0);
			//vec3	a1 = vec3(intersection, 1000);
			//vec3	b0 = startPos;
			//vec3	b1 = startPos + 1000 * reflectDir;
			//vec3	pa, pb;
			//float	mua, mub;
			//LineSegmentsIntersect(a0, a1, b0, b1, pa, pb, mua, mub);
			//endPos = startPos + reflectDir * distance(startPos, pb);
			//endPos = startPos + reflectDir * distance(c, intersection) / 2.828427;
			//reflectDir *= (distance(c, intersection) / 2.828427);
			break;
		}
	}
	return (vec4(1, 0, 0, 0)); */
	//vec3	endPos = hitPos + (reflectDir * ac * 30.f);
	//vec3	endPos = hitPos + (reflectDir * 30.f) * Frag.Depth;
	/* vec2	nReflectDir = normalize(reflectDir.xy);
	float	aA = atan(nReflectDir.x, nReflectDir.y) * 180.f / M_PI;
	float	cA = 180 - (90 + aA);
	vec2	a = Frag.UV;
	vec2	b = sign(a);
	float	ac = length(a - b) / sin(cA * M_PI / 180.f);
	reflectDir *= Frag.Depth; */
	//reflectDir *= max(0.1, Frag.Depth * 0.25);
	//endPos = startPos + reflectDir * maxSteps * Frag.Depth;

	vec3	V = normalize(Frag.Position - Camera.Position);
	vec3	reflectDir = reflect(V, Frag.Normal);
	float	curLength = 0.25;

	vec4	ret = vec4(0, 0, 1, 0);
	for (int i = 0; i < maxSteps; i++)
	{
		vec3	curPos = Frag.Position + reflectDir * curLength;
		vec3	curUV = UVFromPosition(curPos);
		float	curDepth = texture(LastDepth, curUV.xy).r;
		if (curDepth == 1)
			continue;
		vec3	newPos = Position(curUV.xy);
		vec3	tempPos = newPos;
		for (int i = 0; i < KERNEL_SIZE; i++)
		{
			//if (abs(curUV.z - curDepth) <= 0.001)
			if (distance(tempPos, newPos) <= 0.001)
			{
				ret.xyz = curUV.xyz;
				ret.w = dot(reflectDir, V) /* * (1 - Frag.Material.Roughness) */;
				break;
			}
			curUV.xy = curUV.xy + (poissonDisk[i] * (0.25 * Frag.Material.Roughness));
			curDepth = texture(LastDepth, curUV.xy).r;
			tempPos = Position(curUV.xy, curDepth);
			
		}
		/* if (distance(curPos, newPos) <= 0.001)
		{
			return (vec4(curUV, 1));
		} */
		/* vec3	newPos = Position(curUV.xy);
		if (distance(curPos, newPos) <= 0.001)
		{
			return (vec4(curUV, 1));
		}*/
		/* float	curDepth = texture(Texture.Depth, curUV.xy).r;
		if (abs(curUV.z - curDepth) <= 0.001)
		{
			return (vec4(curUV, 1));
		}
		vec3	newPos = Position(curUV.xy, curDepth); */
		curLength = length(Frag.Position - newPos);
	}
	return (ret);

	/* for (int i = 0; i < maxSteps; i++)
	{
		vec3	curPos = mix(startPos, endPos, (i + 1) / float(maxSteps));
		//curPos = hitPos + reflectDir * ((i + 1) / float(maxSteps));
		projectedCoord = Camera.Matrix.Projection * vec4(curPos, 1.0);
		projectedCoord /= projectedCoord.w;
		projectedCoord = projectedCoord * 0.5 + 0.5;
		if (projectedCoord.x > 1 || projectedCoord.x < 0 || projectedCoord.y > 1 || projectedCoord.y < 0)
			break;
		float sampleDepth = sampleLod(Texture.Depth, projectedCoord.xy, 0.1).r;
		if (sampleDepth == 1)
			continue;
		dDepth = abs(projectedCoord.z - sampleDepth);
		if (dDepth <= 0.001) {
			return vec4(projectedCoord.xyz, ret.w * smoothstep(0, 1, 1 - i / float(maxSteps)));
			//return vec4(BinarySearch(reflectDir, curPos, dDepth), ret.w * smoothstep(0, 1, 1 - i / float(maxSteps)));
			break;
		}
	}
	return (ret); */
}

float	Env_Specular(in float NdV, in float roughness)
{
	float	alpha = roughness * roughness;
	float	den = (alpha - 1) + 1;
	float	D = (alpha / (M_PI * den * den));
	float	alpha2 = alpha * alpha;
	float	G = (2 * NdV) / (NdV + sqrt(alpha2 + (1 - alpha2) * (NdV * NdV)));
	return (D * G);
}

vec3	Fresnel(in float factor, in vec3 F0, in float roughness)
{
	return ((max(vec3(1 - roughness), F0)) * pow(max(0, 1 - factor), 5) + F0);
}

void	ApplyTechnique()
{
	const vec3	EnvDiffuse = texture(Texture.Environment.Diffuse, Frag.CubeUV).rgb;

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

	const vec2	BRDF = BRDF(NdV, Frag.Material.Roughness);

	vec3	diffuse = Frag.Material.AO * (sampleLod(Texture.Environment.Diffuse, -N, Frag.Material.Roughness + 0.9).rgb
			+ texture(Texture.Environment.Irradiance, -N).rgb);
	vec3	reflection = sampleLod(Texture.Environment.Diffuse, R, Frag.Material.Roughness * 2.f).rgb;
	vec3	specular = texture(Texture.Environment.Irradiance, R).rgb;
	vec3	reflection_spec = reflection;


	float	brightness = 0;

	if (Frag.Material.Alpha == 0) {
	#ifdef TRANSPARENT
		return ;
	#endif //TRANSPARENT
		Out.Color = vec4(EnvDiffuse, 1);
		brightness = dot(pow(Out.Color.rgb, envGammaCorrection), brightnessDotValue);
		Out.Emitting = max(vec3(0), (Out.Color.rgb - 0.8) * min(1, brightness));
		return ;
	}
	vec3	fresnel = Fresnel(NdV, Frag.Material.Specular, Frag.Material.Roughness);
	reflection *= fresnel;
	brightness = dot(pow(reflection_spec, envGammaCorrection), brightnessDotValue);
	reflection_spec *= brightness * min(fresnel + 1, fresnel * Env_Specular(NdV, Frag.Material.Roughness));
	specular *= fresnel * BRDF.x + mix(vec3(1), fresnel, Frag.Material.Metallic) * BRDF.y;
	//specular += reflection_spec;
	diffuse *= Frag.Material.Albedo.rgb * (1 - Frag.Material.Metallic);

	float	alpha = Frag.Material.Alpha + max(specular.r, max(specular.g, specular.b));
	alpha = min(1, alpha);

	vec3	envReflection = (specular + reflection_spec + reflection) * alpha;

	vec4	ssrResult = vec4(0, 0, 1, 0);
	if (Frag.Material.Roughness < 1) {
		ssrResult = SSR();
	}
	if (ssrResult.w > 0) {
		vec3	ssrReflection = sampleLod(LastColor, ssrResult.xy, Frag.Material.Roughness * 2).rgb + sampleLod(LastEmitting, ssrResult.xy, Frag.Material.Roughness).rgb;
		float	screenEdgeFactor = smoothstep(0, 1, 1 - pow(length(ssrResult.xy * 2 - 1), 10));
		float	reflectionFactor = ssrResult.w * screenEdgeFactor;
		Out.Color.rgb += mix(envReflection, ssrReflection * fresnel, clamp(reflectionFactor, 0, 1));
	}
	else {
		Out.Color.rgb += envReflection;
	}
	Out.Color.rgb += (diffuse + Frag.Material.Emitting) * alpha;
	Out.Color.a = 1;
	//vec4 ssr = SSR();
	//Out.Color.rgb = ssr.xyz * ssr.w;
	Out.Emitting.rgb += max(vec3(0), Out.Color.rgb - 1) + Frag.Material.Emitting;
}

#define EPSILON 0.0001

bool	isZero(in float v)
{
	return (abs(v) < EPSILON);
}

bool	isZero(in vec2 v)
{
	bvec2	eq = equal(v, vec2(0));
	return (eq.x && eq.y);
}

bool	lequal(in vec2 v, in vec2 v1)
{
	bvec2	eq = lessThanEqual(v, v1);
	return (eq.x && eq.y);
}

bool	LineSegmentsIntersect(
   in vec3 p1, in vec3 p2, in vec3 p3, in vec3 p4, out vec3 pa, out vec3 pb,
   out float mua, out float mub)
{
   vec3		p13, p43, p21;
   float	d1343, d4321, d1321, d4343, d2121;
   float	numer, denom;

   p13.x = p1.x - p3.x;
   p13.y = p1.y - p3.y;
   p13.z = p1.z - p3.z;
   p43.x = p4.x - p3.x;
   p43.y = p4.y - p3.y;
   p43.z = p4.z - p3.z;
   if (isZero(p43.x) && isZero(p43.y) && isZero(p43.z))
      return(false);
   p21.x = p2.x - p1.x;
   p21.y = p2.y - p1.y;
   p21.z = p2.z - p1.z;
   if (isZero(p21.x) && isZero(p21.y) && isZero(p21.z))
      return(false);

   d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
   d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
   d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
   d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
   d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;

   denom = d2121 * d4343 - d4321 * d4321;
   if (isZero(denom))
      return(false);
   numer = d1343 * d4321 - d1321 * d4343;

   mua = numer / denom;
   mub = (d1343 + d4321 * mua) / d4343;

   pa.x = p1.x + mua * p21.x;
   pa.y = p1.y + mua * p21.y;
   pa.z = p1.z + mua * p21.z;
   pb.x = p3.x + mub * p43.x;
   pb.y = p3.y + mub * p43.y;
   pb.z = p3.z + mub * p43.z;

   return(true);
}

bool	LineSegmentsIntersect(in vec3 a0, in vec3 a1, in vec3 b0, in vec3 b1, out vec3 ip)
{
	vec3 da = a1 - a0; 
	vec3 db = b1 - b0;
	vec3 dc = b0 - a0;

	if (dot(dc, cross(da, db)) != 0.0) // lines are not coplanar
		return (false);

	vec3	dadb = cross(da, db);
	float	s = dot(cross(dc, db), cross(da, db)) / dot(dadb, dadb);
	if (s >= 0.0 && s <= 1.0)
	{
		da = normalize(da);
		ip = a0 + da * vec3(s, s, s);
		return (true);
	}
	return (false);
}

float	cross(in vec2 v, in vec2 v1)
{
	return (v.x * v1.y - v.y * v1.x);
}

//https://www.codeproject.com/tips/862988/find-the-intersection-point-of-two-line-segments
bool	LineSegmentsIntersect(in vec2 p, in vec2 p2, in vec2 q, in vec2 q2, out vec2 intersection)
{
	vec2	r = p2 - p;
	vec2	s = q2 - q;
	float	rxs = cross(r, s);
	float	qpxr = cross(q - p, r);

	if (isZero(rxs) && isZero(qpxr)) {
		return (false);
	}
	if (isZero(rxs) && !isZero(qpxr))
	return (false);
	float	t = cross(q - p, s) / rxs;
	float	u = cross(q - p, r) / rxs;
	if (!isZero(rxs) && (0 <= t && t <= 1) && (0 <= u && u <= 1))
	{
		intersection = p + t*r;
		return (true);
	}
	return (false);
}

/* bool	LineSegmentsIntersect(in vec2 rayOrigin, in vec2 rayDirection, in vec2 point1, in vec2 point2, out vec2 inter)
{
	vec2	v1 = rayOrigin - point1;
	vec2	v2 = point2 - point1;
	vec2	v3 = vec2(-rayDirection.y, rayDirection.x);
	float	d = dot(v2, v3);

	if (isZero(d))
		return (false);

	float t1 = cross(v2, v1) / d;
	float t2 = dot(v1, v3) / d;

	if (t1 >= 0.0 && (t2 >= 0.0 && t2 <= 1.0)) {
		inter = rayOrigin + t1;
		return (true);
	}

	return (false);
} */
