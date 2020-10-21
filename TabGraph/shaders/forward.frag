R""(
#define M_PI 3.1415926535897932384626433832795
#if defined(TEXTURE_USE_HEIGHT) || defined(TEXTURE_USE_DIFFUSE) || defined(TEXTURE_USE_EMITTING) || defined(TEXTURE_USE_NORMAL) || defined(TEXTURE_USE_AO)
#define USE_TEXTURES
#endif

struct t_Matrix {
	mat4	Model;
	mat4	Normal;
};

struct t_Environment {
	samplerCube	Diffuse;
	samplerCube	Irradiance;
};

#ifdef USE_TEXTURES
struct t_StandardTextures {
#ifdef TEXTURE_USE_HEIGHT
	sampler2D	Height;
#endif
#ifdef TEXTURE_USE_DIFFUSE
	sampler2D	Diffuse;
#endif
#ifdef TEXTURE_USE_EMITTING
	sampler2D	Emitting;
#endif
#ifdef TEXTURE_USE_NORMAL
	sampler2D	Normal;
#endif
#ifdef TEXTURE_USE_AO
	sampler2D	AO;
#endif
};
#endif

struct t_StandardValues {
	vec3		Diffuse;
	vec3		Emitting;
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
	mat4	ViewProjection;
};

struct t_Camera {
	vec3			Position;
	t_CameraMatrix	Matrix;
	t_CameraMatrix	InvMatrix;
};

uniform t_Camera			Camera;
#ifdef USE_TEXTURES
uniform t_StandardTextures	StandardTextures;
#endif
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

layout(location = 0) out vec4	out_CDiff; //BRDF CDiff, Transparency
layout(location = 1) out vec3	out_Emitting;
layout(location = 2) out vec4	out_F0; //BRDF F0, BRDF Alpha
layout(location = 3) out float	out_Ior;
layout(location = 4) out float	out_AO;
layout(location = 5) out vec2	out_Normal;

#define Opacity() (out_CDiff.a)
#define SetOpacity(opacity) (out_CDiff.a = opacity)

#define CDiff() (out_CDiff.rgb)
#define SetCDiff(cDiff) (out_CDiff.rgb = cDiff)

#define F0() (out_F0.rgb)
#define SetF0(f0) (out_F0.rgb = f0)

#define Alpha() (out_F0.a)
#define SetAlpha(alpha) (out_F0.a = alpha)

#define Emitting() (out_Emitting)
#define SetEmitting(emitting) (out_Emitting = emitting)

#define Ior() (out_Ior)
#define SetIor(ior) (out_Ior = ior)

#define AO() (out_AO)
#define SetAO(aO) (out_AO = aO)

#define EncodedNormal() (out_Normal)
#define SetEncodedNormal(normal) (out_Normal = normal)

t_Frag	Frag;

#define map(value, low1, high1, low2, high2) (low2 + (value - low1) * (high2 - low2) / (high1 - low1))

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
	SetIor(StandardValues.Ior);
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
#ifdef TEXTURE_USE_EMITTING
	vec3 emitting = texture(StandardTextures.Emitting, TexCoord()).rgb;
	SetEmitting(StandardValues.Emitting * emitting);
#else
	SetEmitting(StandardValues.Emitting);
#endif
#ifdef TEXTURE_USE_AO
	SetAO(StandardValues.AO + texture(StandardTextures.AO, TexCoord()).r);
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

/*void	FillOut()
{
	out_CDiff = vec4(Frag.BRDF.CDiff, StandardValues.Opacity);
	out_F0.rgb = Frag.BRDF.F0;
	out_F0.a = Frag.BRDF.Alpha;
	out_Emitting = max(vec3(0), StandardValues.Emitting + StandardValues.Diffuse.rgb - 1);
	out_Ior = StandardValues.Ior;
	out_AO = StandardValues.AO;
	out_Normal = encodeNormal(normalize(WorldNormal()));
#ifdef FORCEDEPTHWRITE
	gl_FragDepth = Frag.Depth;
	bvec3	positionsEqual = notEqual(WorldPosition(), frag_WorldPosition);
	if (positionsEqual.x || positionsEqual.y || positionsEqual.z)
	{
		vec4	NDC = Camera.Matrix.Projection * Camera.Matrix.View * vec4(WorldPosition(), 1.0);
		gl_FragDepth = NDC.z / NDC.w * 0.5 + 0.5;
	}
#endif //FORCEDEPTHWRITE
}*/
)""