R""(
#define M_PI 3.1415926535897932384626433832795
#define Luminance(linearColor) dot(linearColor, vec3(0.299, 0.587, 0.114))

/*#if defined(TEXTURE_USE_HEIGHT) || defined(TEXTURE_USE_DIFFUSE) || defined(TEXTURE_USE_EMISSIVE) || defined(TEXTURE_USE_NORMAL) || defined(TEXTURE_USE_AO)
#define USE_TEXTURES
#endif*/

#ifndef textureQueryLevels
float compMax(vec3 v) { return max(max(v.x, v.y), v.z); }
float compMax(vec2 v) { return max(v.x, v.y); }
#define textureQueryLevels(tex) int(log2(compMax(textureSize(tex, 0))))
#endif

#define sampleLod(tex, uv, lod) textureLod(tex, uv, lod * textureQueryLevels(tex))

struct t_Matrix {
	mat4	Model;
	mat4	Normal;
};

struct t_Environment {
	samplerCube	Diffuse;
	samplerCube	Irradiance;
};

struct t_StandardTextures {
#ifdef TEXTURE_USE_HEIGHT
	sampler2D	Height;
#endif
#ifdef TEXTURE_USE_DIFFUSE
	sampler2D	Diffuse;
#endif
#ifdef TEXTURE_USE_EMISSIVE
	sampler2D	Emissive;
#endif
#ifdef TEXTURE_USE_NORMAL
	sampler2D	Normal;
#endif
#ifdef TEXTURE_USE_AO
	sampler2D	AO;
#endif
	sampler2D	BRDFLUT;
};

#define OPAQUE	0
#define MASK	1
#define BLEND	2

struct t_StandardValues {
	vec3		Diffuse;
	vec3		Emissive;
	int			OpacityMode;
	float		OpacityCutoff;
	float		Opacity;
	float		Parallax;
	float		Ior;
	float		AO;
};

struct t_BRDF {
	vec3	CDiff;
	vec3	F0;
	float	Alpha;
};

struct t_Frag {
	float		Depth;
	vec2		TexCoord;
	vec3		WorldPosition;
	vec3		WorldNormal;
};

struct t_CameraMatrix {
	mat4	View;
	mat4	Projection;
};

struct t_Camera {
	vec3			Position;
	t_CameraMatrix	Matrix;
	t_CameraMatrix	InvMatrix;
};

uniform t_Camera			Camera;
uniform t_StandardTextures	StandardTextures;
uniform t_StandardValues	StandardValues;
uniform t_Matrix			Matrix;
uniform t_Environment		Environment;
uniform vec3				Resolution;
uniform float				Time;

in VertexData {
	vec3	WorldPosition;
	vec3	WorldNormal;
	vec2	TexCoord;
} Input;

#ifdef GEOMETRY
layout(location = 0) out vec4	out_CDiff; //BRDF CDiff, Transparency
layout(location = 1) out vec3	out_Emissive;
layout(location = 2) out vec4	out_F0; //BRDF F0, BRDF Alpha
layout(location = 3) out float	out_AO;
layout(location = 4) out vec2	out_Normal;
#elif defined(MATERIAL) || defined(DEPTH)
vec4	out_CDiff; //BRDF CDiff, Transparency
vec3	out_Emissive;
vec4	out_F0; //BRDF F0, BRDF Alpha
float	out_AO;
vec2	out_Normal;
#endif

#define ScreenTexCoord() (gl_FragCoord.xy / Resolution.xy)

#define Opacity() (out_CDiff.a)
#define SetOpacity(opacity) (out_CDiff.a = opacity)

#define CDiff() (out_CDiff.rgb)
#define SetCDiff(cDiff) (out_CDiff.rgb = cDiff)

#define F0() (out_F0.rgb)
#define SetF0(f0) (out_F0.rgb = f0)

#define Alpha() (out_F0.a)
#define SetAlpha(alpha) (out_F0.a = alpha)

#define Emissive() (out_Emissive)
#define SetEmissive(emissive) (out_Emissive = emissive)

#define Ior() (StandardValues.Ior)

#define AO() (out_AO)
#define SetAO(aO) (out_AO = aO)

#define EncodedNormal() (out_Normal)
#define SetEncodedNormal(normal) (out_Normal = normal)

#define BRDF(NdV, Roughness) (texture(StandardTextures.BRDFLUT, vec2(NdV, Roughness)).xy)

t_Frag	Frag;

#define map(value, low1, high1, low2, high2) (low2 + (value - low1) * (high2 - low2) / (high1 - low1))

vec2 encodeNormal(vec3 n) {
	float p = sqrt(n.z*8+8);
    return n.xy/p + 0.5;
}

vec3 decodeNormal(vec2 enc) {
	vec2 fenc = enc*4-2;
    float f = dot(fenc,fenc);
    float g = sqrt(1-f/4);
    vec3 n;
    n.xy = fenc*g;
    n.z = 1-f/2;
    return n;
}

/*
vec2 sign_not_zero(vec2 v) {
    return fma(step(vec2(0.0), v), vec2(2.0), vec2(-1.0));
}

vec2 encodeNormal(vec3 v) {
	v = map(v, vec3(-1), vec3(1), vec3(-1), vec3(0.9));
	//v += 0.05;
	//v = v * 0.5 + 0.5;
	// Faster version using newer GLSL capatibilities
	v.xy /= dot(abs(v), vec3(1));
	// Branch-Less version
	v.xy = mix(v.xy, (1.0 - abs(v.yx)) * sign_not_zero(v.xy), step(v.z, 0.0));
	return v.xy;// * 0.5 + 0.5;
}

vec3 decodeNormal(vec2 packed_nrm) {
	//packed_nrm = packed_nrm * 2 - 1;
    // Version using newer GLSL capatibilities
    vec3 v = vec3(packed_nrm.xy, 1.0 - abs(packed_nrm.x) - abs(packed_nrm.y));
    #if 1
        // Version with branches, seems to take less cycles than the
        // branch-less version
        if (v.z < 0)
        	v.xy = (1.0 - abs(v.yx)) * sign_not_zero(v.xy);
    #else
        // Branch-Less version
        v.xy = mix(v.xy, (1.0 - abs(v.yx)) * sign_not_zero(v.xy), step(v.z, 0));
    #endif
    return map(v, vec3(-1), vec3(0.9), vec3(-1), vec3(1));
}
 */
/*vec3 decodeNormal(in vec2 f)
{
	vec3 n;
	n.xy = -enc*enc+enc;
	n.z = -1;
	float f = dot(n, vec3(1,1,0.25));
	float m = sqrt(f);
	n.xy = (enc*8-4) * m;
	n.z = 1 - 8*f;
	return n;
}*/

bool _WorldPositionSet = false;
bool _WorldNormalSet = false;
bool _TexCoordSet = false;
bool _DepthSet = false;

void SetWorldPosition(in vec3 worldPosition)
{
	Frag.WorldPosition = worldPosition;
	_WorldPositionSet = true;
}

vec3 WorldPosition()
{
	if (!_WorldPositionSet)
		SetWorldPosition(Input.WorldPosition);
	return Frag.WorldPosition;
}

void SetWorldNormal(in vec3 worldNormal)
{
	Frag.WorldNormal = normalize(worldNormal);
	SetEncodedNormal(encodeNormal(Frag.WorldNormal));
	_WorldNormalSet = true;
}

vec3 WorldNormal()
{
	if (!_WorldNormalSet)
		SetWorldNormal(Input.WorldNormal);
	return Frag.WorldNormal;
}

void SetTexCoord(vec2 texCoord)
{
	Frag.TexCoord = texCoord;
	_TexCoordSet = true;
}

vec2 TexCoord()
{
	if (!_TexCoordSet)
		SetTexCoord(Input.TexCoord);
	return Frag.TexCoord;
}

void SetDepth(in float depth)
{
	Frag.Depth = depth;
#ifdef FORCEDEPTHWRITE
	gl_FragDepth = depth;
#endif //FORCEDEPTHWRITE
	_DepthSet = true;
}

float Depth()
{
	if (!_DepthSet)
		SetDepth(gl_FragCoord.z);
	return Frag.Depth;
}

#define Parallax() (StandardValues.Parallax)

#ifdef TEXTURE_USE_HEIGHT
void	Parallax_Mapping(in vec3 tbnV, out float parallaxHeight)
{
	vec2 T = TexCoord();
	const float minLayers = 10;
	const float maxLayers = 15;
	float numLayers = mix(maxLayers, minLayers, abs(tbnV.z));
	int	tries = int(numLayers);
	float layerHeight = 1.0 / numLayers;
	float curLayerHeight = 0;
	vec2 dtex = Parallax() * tbnV.xy / tbnV.z / numLayers;
	vec2 currentTextureCoords = T;
	float heightFromTexture = 1 - texture(StandardTextures.Height, currentTextureCoords).r;
	while(tries > 0 && heightFromTexture > curLayerHeight) 
	{
		tries--;
		curLayerHeight += layerHeight; 
		currentTextureCoords -= dtex;
		heightFromTexture = 1 - texture(StandardTextures.Height, currentTextureCoords).r;
	}
	vec2 prevTCoords = currentTextureCoords + dtex;
	float nextH	= heightFromTexture - curLayerHeight;
	float prevH	= 1 - texture(StandardTextures.Height, prevTCoords).r
	- curLayerHeight + layerHeight;
	float weight = nextH / (nextH - prevH);
	vec2 finalTexCoords = prevTCoords * weight + currentTextureCoords * (1.0 - weight);
	parallaxHeight = (curLayerHeight + prevH * weight + nextH * (1.0 - weight));
	parallaxHeight *= Parallax();
	parallaxHeight = isnan(parallaxHeight) ? 0 : parallaxHeight;
	SetTexCoord(finalTexCoords);
}
#endif

mat3x3	tbn_matrix()
{
	vec3 Q1 = dFdx(WorldPosition());
	vec3 Q2 = dFdy(WorldPosition());
	vec2 st1 = dFdx(TexCoord());
	vec2 st2 = dFdy(TexCoord());
	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = normalize(-Q1*st2.s + Q2*st1.s);
	return(transpose(mat3(T, B, WorldNormal())));
}

void	FillFragmentData()
{
	SetF0(vec3(0.04f));
	SetAlpha(1.f);
	SetWorldNormal(Input.WorldNormal);
	vec3 viewDir = normalize(Camera.Position - WorldPosition());
	if (dot(viewDir, WorldNormal()) < 0)
		SetWorldNormal(-WorldNormal());
#if defined(TEXTURE_USE_HEIGHT) || defined(TEXTURE_USE_NORMAL)
	mat3	tbn = tbn_matrix();
#endif
#ifdef TEXTURE_USE_HEIGHT
	float ph = 0;
	Parallax_Mapping(tbn * viewDir, ph);
#endif
#ifdef TEXTURE_USE_DIFFUSE
	vec4	albedo_sample = texture(StandardTextures.Diffuse, TexCoord());
	SetCDiff(StandardValues.Diffuse * albedo_sample.rgb);
	SetOpacity(StandardValues.Opacity * albedo_sample.a);
#else
	SetCDiff(StandardValues.Diffuse);
	SetOpacity(StandardValues.Opacity);
#endif
#ifdef TEXTURE_USE_EMISSIVE
	vec3 emissive = texture(StandardTextures.Emissive, TexCoord()).rgb;
	SetEmissive(StandardValues.Emissive * emissive);
#else
	SetEmissive(StandardValues.Emissive);
#endif
#ifdef TEXTURE_USE_AO
	SetAO(StandardValues.AO + max(1 - texture(StandardTextures.AO, TexCoord()).r, 0));
#else
	SetAO(StandardValues.AO);
#endif
#ifdef TEXTURE_USE_NORMAL
	vec3	normal_sample = texture(StandardTextures.Normal, TexCoord()).xyz * 2 - 1;
	vec3	new_normal = normal_sample * tbn;
	SetWorldNormal(normalize(new_normal));
#endif
#ifdef TEXTURE_USE_HEIGHT
	SetWorldPosition(WorldPosition() - (WorldNormal() * ph));
#endif

}
)""