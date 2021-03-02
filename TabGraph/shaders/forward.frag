R""(
#define M_PI 3.1415926535897932384626433832795
#define Luminance(linearColor) dot(linearColor, vec3(0.299, 0.587, 0.114))

#ifndef textureQueryLevels
float compMax(vec3 v) { return max(max(v.x, v.y), v.z); }
float compMax(vec2 v) { return max(v.x, v.y); }
#define textureQueryLevels(tex) int(log2(compMax(textureSize(tex, 0))))
#endif

#define sampleLod(tex, uv, lod) textureLod(tex, uv, lod * textureQueryLevels(tex))

struct t_Matrix {
	mat4	Model;
	mat3	Normal;
};

struct t_Environment {
	samplerCube	Diffuse;
	samplerCube	Irradiance;
};

#define OPAQUE	0
#define MASK	1
#define BLEND	2

struct t_BRDF {
	vec3	CDiff;
	vec3	F0;
	float	Alpha;
};

struct t_Frag {
	float		Depth;
	vec2		TexCoord;
	vec3		WorldPosition;
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
uniform t_Camera			PrevCamera;
#ifdef TEXTURE_USE_HEIGHT
uniform sampler2D			StandardTextureHeight;
#endif
#ifdef TEXTURE_USE_DIFFUSE
uniform sampler2D			StandardTextureDiffuse;
#endif
#ifdef TEXTURE_USE_EMISSIVE
uniform sampler2D			StandardTextureEmissive;
#endif
#ifdef TEXTURE_USE_NORMAL
uniform sampler2D			StandardTextureNormal;
#endif
#ifdef TEXTURE_USE_AO
uniform sampler2D			StandardTextureAO;
#endif
uniform sampler2D			StandardTextureBRDFLUT;
uniform vec3				StandardDiffuse = vec3(1);
uniform vec3				StandardEmissive = vec3(0);
uniform float				StandardOpacityCutoff = float(0.5);
uniform float				StandardOpacity = float(1);
uniform float				StandardParallax = float(0.05);
uniform float				StandardIor = float(1);
uniform float				StandardAO = float(0);
uniform t_Matrix			Matrix;
uniform t_Environment		Environment;
uniform vec3				Resolution;
uniform float				Time;
uniform uint				FrameNumber;

in VertexData {
	vec3	WorldPosition;
	vec3	WorldNormal;
	vec2	TexCoord;
} Input;

in vec4 Position;
in vec4 PreviousPosition;

#ifdef GEOMETRY
layout(location = 0) out vec4	_CDiff; //BRDF CDiff, Transparency
layout(location = 1) out vec3	_Emissive;
layout(location = 2) out vec4	_F0; //BRDF F0, BRDF Alpha
layout(location = 3) out float	_AO;
layout(location = 4) out vec3	_Normal;
layout(location = 5) out uint	_InstanceID;
layout(location = 6) out vec2	_Velocity;
#elif defined(MATERIAL) || defined(DEPTH)
vec4	_CDiff; //BRDF CDiff, Transparency
vec3	_Emissive;
vec4	_F0; //BRDF F0, BRDF Alpha
float	_AO;
vec3	_Normal;
uint	_InstanceID;
vec2	_Velocity;
#endif

#define ScreenTexCoord() (gl_FragCoord.xy / Resolution.xy)

#define OpacityCutoff() (StandardOpacityCutoff)

#define Opacity() (_CDiff.a)
#define SetOpacity(opacity) (_CDiff.a = opacity)

#define CDiff() (_CDiff.rgb)
#define SetCDiff(cDiff) (_CDiff.rgb = cDiff)

#define F0() (_F0.rgb)
#define SetF0(f0) (_F0.rgb = f0)

#define Alpha() (_F0.a)
#define SetAlpha(alpha) (_F0.a = alpha)

#define Emissive() (_Emissive)
#define SetEmissive(emissive) (_Emissive = emissive)

#define Ior() (StandardIor)

#define AO() (_AO)
#define SetAO(aO) (_AO = aO)

#define BRDF(NdV, Roughness) (texture(StandardTextureBRDFLUT, vec2(NdV, Roughness)).xy)

t_Frag	Frag;

#define map(value, low1, high1, low2, high2) (low2 + (value - low1) * (high2 - low2) / (high1 - low1))

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
	_Normal = normalize(worldNormal);
	_WorldNormalSet = true;
}

vec3 WorldNormal()
{
	if (!_WorldNormalSet)
		SetWorldNormal(Input.WorldNormal);
	return _Normal;
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

#define Parallax() (StandardParallax)

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
	float heightFromTexture = 1 - texture(StandardTextureHeight, currentTextureCoords).r;
	while(tries > 0 && heightFromTexture > curLayerHeight) 
	{
		tries--;
		curLayerHeight += layerHeight; 
		currentTextureCoords -= dtex;
		heightFromTexture = 1 - texture(StandardTextureHeight, currentTextureCoords).r;
	}
	vec2 prevTCoords = currentTextureCoords + dtex;
	float nextH	= heightFromTexture - curLayerHeight;
	float prevH	= 1 - texture(StandardTextureHeight, prevTCoords).r
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
	vec3 a = Position.xyz / Position.w * 0.5 + 0.5;
	vec3 b = PreviousPosition.xyz / PreviousPosition.w * 0.5 + 0.5;
	_Velocity = b.xy - a.xy;
	SetF0(vec3(0.04f));
	SetAlpha(1.f);
	SetWorldNormal(Input.WorldNormal);
	SetDepth(gl_FragCoord.z);
#if defined(TEXTURE_USE_HEIGHT) || defined(TEXTURE_USE_NORMAL)
	mat3	tbn = tbn_matrix();
#endif
#ifdef TEXTURE_USE_HEIGHT
	float ph = 0;
	vec3 viewDir = normalize(Camera.Position - WorldPosition());
	Parallax_Mapping(tbn * viewDir, ph);
#endif
#ifdef TEXTURE_USE_DIFFUSE
	vec4	albedo_sample = texture(StandardTextureDiffuse, TexCoord());
	SetCDiff(StandardDiffuse * albedo_sample.rgb);
	SetOpacity(StandardOpacity * albedo_sample.a);
#else
	SetCDiff(StandardDiffuse);
	SetOpacity(StandardOpacity);
#endif
#ifdef TEXTURE_USE_EMISSIVE
	vec3 emissive = texture(StandardTextureEmissive, TexCoord()).rgb;
	SetEmissive(StandardEmissive * emissive);
#else
	SetEmissive(StandardEmissive);
#endif
#ifdef TEXTURE_USE_AO
	SetAO(StandardAO + max(1 - texture(StandardTextureAO, TexCoord()).r, 0));
#else
	SetAO(StandardAO);
#endif
#ifdef TEXTURE_USE_NORMAL
	vec3	normal_sample = texture(StandardTextureNormal, TexCoord()).xyz * 2 - 1;
	vec3	new_normal = normal_sample * tbn;
	SetWorldNormal(normalize(new_normal));
#endif
#ifdef TEXTURE_USE_HEIGHT
	SetWorldPosition(WorldPosition() - (WorldNormal() * ph));
#endif
}

)""