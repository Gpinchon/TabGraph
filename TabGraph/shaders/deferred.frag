R""(
#define M_PI	3.1415926535897932384626433832795
#define PI		M_PI
#define EPSILON	0.0001
#define lequal(a, b) all(lessThanEqual(a, b))
#define Luminance(linearColor) dot(linearColor, vec3(0.299, 0.587, 0.114))

struct t_Environment {
	samplerCube	Diffuse;
	samplerCube	Irradiance;
};

struct t_Frag {
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

struct t_BRDF {
	vec3	CDiff;
	vec3	F0;
	float	Alpha;
};

struct t_GeometryTextures {
	sampler2D		CDiff;
	sampler2D		F0;
	sampler2D		Normal;
	sampler2D		Depth;
	sampler2D		Emissive;
	sampler2D		AO;
	isampler2D		ID;
};

in VertexData {
	vec2	TexCoord;
	vec3	CubeTexCoord;
} Input;

vec2 TexCoord() {
	return Input.TexCoord;
}

vec3 CubeTexCoord() {
	return Input.CubeTexCoord;
}

struct t_Textures {
	t_GeometryTextures	Geometry;
	t_Environment		Environment;
};

#define map(value, low1, high1, low2, high2) (low2 + (value - low1) * (high2 - low2) / (high1 - low1))

uniform vec3			Resolution;
uniform float			Time;
uniform uint			FrameNumber;
uniform t_Camera		PrevCamera;
uniform t_Camera		Camera;
uniform t_Textures		Texture;

#ifdef LIGHTSHADER
	layout(location = 0) out vec4	out_0;
	layout(location = 1) out vec4	out_1;
	layout(location = 2) out vec4	out_2;
	vec4	_CDiff; //BRDF CDiff, Transparency
	vec3	_Emissive;
	vec4	_F0; //BRDF F0, BRDF Alpha
	float	_AO;
	vec3	_Normal;
#endif //LIGHTSHADER

#ifdef POSTSHADER
	layout(location = 0) out vec4	_CDiff; //BRDF CDiff, Transparency
	layout(location = 1) out vec3	_Emissive;
	layout(location = 2) out vec4	_F0; //BRDF F0, BRDF Alpha
	layout(location = 3) out float	_AO;
	layout(location = 4) out vec3	_Normal;
#endif //POSTSHADER

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
	
#define AO() (_AO)
#define SetAO(ao) (_AO = ao)

#define WorldNormal() (_Normal)
#define SetWorldNormal(normal) (_Normal = normalize(normal))

float _depth;
float Depth() { return _depth; }
float Depth(vec2 uv) { return textureLod(Texture.Geometry.Depth, uv, 0).r; }
#define SetDepth(depth) (gl_FragDepth = depth)

#define ScreenTexCoord() (gl_FragCoord.xy / Resolution.xy)

t_Frag	Frag;

float SceneDepth(vec2 UV, float depth)
{
	vec4	projectedCoord = vec4(UV * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	projectedCoord = Camera.InvMatrix.View * Camera.InvMatrix.Projection * projectedCoord;
	return projectedCoord.w;
}

mat3x3	tbn_matrix()
{
	vec3 Q1 = dFdx(gl_FragCoord.xyz);
	vec3 Q2 = dFdy(gl_FragCoord.xyz);
	vec2 st1 = dFdx(TexCoord());
	vec2 st2 = dFdy(TexCoord());
	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = normalize(-Q1*st2.s + Q2*st1.s);
	return(transpose(mat3(T, B, WorldNormal())));
}

vec4	WorldToView(in vec3 position)
{
	return Camera.Matrix.View * vec4(position, 1);
}

vec4	ViewToClip(in vec4 position)
{
	return Camera.Matrix.Projection * position;
}

vec4	ClipToScreen(in vec4 position)
{
	position.xyz /= position.w;
	position.xyz = position.xyz * 0.5 + 0.5;
	return position;
}

/** Returns the World position and the scene depth in world units */
vec4	WorldToScreen(in vec3 position)
{
	return ClipToScreen(ViewToClip(WorldToView(position)));
}

vec4	WorldToClip(in vec3 position)
{
	return ViewToClip(WorldToView(position));
}

vec3 TangentToWorld(in vec3 vec)
{
	return normalize(vec * tbn_matrix());
}

vec3	TangentToWorld(in vec2 vec)
{
	return TangentToWorld(vec3(vec, 1));
}

vec3	ScreenToWorld(in vec2 uv, in float depth)
{
	vec4	projectedCoord = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	projectedCoord = Camera.InvMatrix.View * Camera.InvMatrix.Projection * projectedCoord;
	return (projectedCoord.xyz / projectedCoord.w);
}

#define SetWorldPosition(worldPosition) (gl_FragDepth = WorldToScreen(worldPosition).z)

vec3	WorldPosition(in vec2 uv)
{
	return ScreenToWorld(uv, Depth(uv));
}

vec3 WorldPosition()
{
	return ScreenToWorld(ScreenTexCoord(), Depth());
}

//Generate Pseudo random numbers using TEA (Tiny Encription Algorithm)
uvec2 ScrambleTEA(uvec2 v, uint IterationCount)
{
	// Start with some random data (numbers can be arbitrary but those have been used by others and seem to work well)
	uint k[4] ={ 0xA341316Cu , 0xC8013EA4u , 0xAD90777Du , 0x7E95761Eu };
	
	uint y = v[0];
	uint z = v[1];
	uint sum = 0;
	
	for(uint i = 0; i < IterationCount; ++i)
	{
		sum += 0x9e3779b9;
		y += (z << 4u) + k[0] ^ z + sum ^ (z >> 5u) + k[1];
		z += (y << 4u) + k[2] ^ y + sum ^ (y >> 5u) + k[3];
	}

	return uvec2(y, z);
}

uvec2 ScrambleTEA(uvec2 v)
{
	return ScrambleTEA(v, 3);
}

float	random(in vec2 seed, in float freq)
{
	float dt = dot(floor(seed * freq), vec2(53.1215, 21.1352));
	return fract(sin(dt) * 2105.2354);
}

float	random(in vec3 seed, in float freq)
{
	float dt = dot(floor(seed * freq), vec3(53.1215, 21.1352, 9.1322));
	return fract(sin(dt) * 2105.2354);
}

float	random(in vec2 seed)
{
	return random(seed, 4096);
}

float	random(in vec3 seed)
{
	return random(seed, 4096);
}

float	randomAngle(in vec2 seed)
{
	return random(seed) * 6.283285;
}

float	randomAngle(in vec3 seed)
{
	return random(seed) * 6.283285;
}

#ifndef textureQueryLevels
float compMax(vec3 v) { return max(max(v.x, v.y), v.z); }
float compMax(vec2 v) { return max(v.x, v.y); }
#define textureQueryLevels(tex) int(log2(compMax(textureSize(tex, 0))))
#endif

#define sampleLod(tex, uv, lod) textureLod(tex, uv, lod * textureQueryLevels(tex))

vec4	texelFetchLod(in sampler2D tex, in vec2 uv, in float mipLevel)
{
	ivec2	Resolution = textureSize(tex, 0);
	return mix(texelFetch(tex, ivec2(Resolution * uv), int(floor(mipLevel))), texelFetch(tex, ivec2(Resolution * uv), int(ceil(mipLevel))), fract(mipLevel));
}

vec4	texelFetchLod(in sampler2D tex, in vec2 uv, in int mipLevel)
{
	return texelFetch(tex, ivec2(textureSize(tex, 0) * uv), mipLevel);
}

float	smootherstep(float edge0, float edge1, float x) {
	x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
	return x * x * x * (x * (x * 6 - 15) + 10);
}

bool	isZero(in float v)
{
	return (abs(v) < EPSILON);
}

bool	isZero(in vec2 v)
{
	return all(equal(v, vec2(0)));
}

void FillFragmentData(void)
{
	vec4	CDiff_sample = textureLod(Texture.Geometry.CDiff, ScreenTexCoord(), 0);
	vec4	F0_sample = textureLod(Texture.Geometry.F0, ScreenTexCoord(), 0);
	vec3	Normal_sample = textureLod(Texture.Geometry.Normal, ScreenTexCoord(), 0).xyz;
	float	Depth_sample = textureLod(Texture.Geometry.Depth, ScreenTexCoord(), 0).x;
	vec3	Emissive_sample = textureLod(Texture.Geometry.Emissive, ScreenTexCoord(), 0).rgb;
	float	AO_sample = textureLod(Texture.Geometry.AO, ScreenTexCoord(), 0).x;
	SetCDiff(CDiff_sample.rgb);
	SetOpacity(CDiff_sample.a);
	SetF0(F0_sample.rgb);
	SetAlpha(F0_sample.a);
	SetWorldNormal(Normal_sample);
	SetEmissive(Emissive_sample);
	SetAO(AO_sample);
	//SetWorldNormal(decodeNormal(EncodedNormal()));
	_depth = Depth_sample;
	SetWorldPosition(WorldPosition(ScreenTexCoord()));
}
)""