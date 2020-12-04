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
float _opacity = 1; 
#define Opacity() (_opacity)

vec3 _cdiff = vec3(1);
#define CDiff() (_cdiff)

vec3 _f0 = vec3(0.04);
#define F0() (_f0)

float _alpha = 1;
#define Alpha() (_alpha)

vec3 _emissive = vec3(0);
#define Emissive() (_emissive)

float _ao = 0;
#define AO() (_ao)

vec2 _normal = vec2(0);
#define EncodedNormal() (_normal)

#ifdef LIGHTSHADER
	struct t_Textures {
		t_GeometryTextures	Geometry;
		t_Environment		Environment;
	};
	
	uniform t_Textures		Texture;
	uniform t_Camera		Camera;
	uniform vec3			Resolution;
	uniform float			Time;
	uniform uint			FrameNumber;
	
	layout(location = 0) out vec4	out_0;
	layout(location = 1) out vec4	out_1;

	#define SetOpacity(opacity) (_opacity = opacity)

	#define SetCDiff(cDiff) (_cdiff = cDiff)

	#define SetF0(f0) (_f0 = f0)

	#define SetAlpha(alpha) (_alpha = alpha)

	#define SetEmissive(emissive) (_emissive = emissive)

	#define SetAO(aO) (_ao = aO)

	#define SetEncodedNormal(normal) (_normal = normal)
#endif //LIGHTSHADER

#ifdef POSTSHADER
	struct t_Textures {
		t_GeometryTextures	Geometry;
		t_Environment		Environment;
	};
	layout(location = 0) out vec4	out_CDiff; //BRDF CDiff, Transparency
	layout(location = 1) out vec3	out_Emissive;
	layout(location = 2) out vec4	out_F0; //BRDF F0, BRDF Alpha
	layout(location = 3) out float	out_AO;
	layout(location = 4) out vec2	out_Normal;
	uniform t_Textures		Texture;
	uniform t_Camera		Camera;
	uniform vec3			Resolution;
	uniform float			Time;
	uniform uint			FrameNumber;

	#define SetOpacity(opacity) (out_CDiff.a = opacity)
	
	#define SetCDiff(cDiff) (out_CDiff.rgb = cDiff)
	
	#define SetF0(f0) (out_F0.rgb = f0)
	
	#define SetAlpha(alpha) (out_F0.a = alpha)
	
	#define SetEmissive(emissive) (out_Emissive = emissive)
	
	#define SetAO(aO) (out_AO = aO)
	
	#define SetEncodedNormal(normal) (out_Normal = normal)
#endif //POSTSHADER

float _depth;
float Depth() { return _depth; }
float Depth(vec2 uv) { return textureLod(Texture.Geometry.Depth, uv, 0).r; }
#define SetDepth(depth) (gl_FragDepth = depth)

#define ScreenTexCoord() (gl_FragCoord.xy / Resolution.xy)

t_Frag	Frag;

void SetWorldNormal(in vec3 worldNormal)
{
	SetEncodedNormal(encodeNormal(Frag.WorldNormal));
}

vec3 WorldNormal()
{
	return decodeNormal(EncodedNormal());
}

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
	vec2	Normal_sample = textureLod(Texture.Geometry.Normal, ScreenTexCoord(), 0).xy;
	float	Depth_sample = textureLod(Texture.Geometry.Depth, ScreenTexCoord(), 0).x;
	vec3	Emissive_sample = textureLod(Texture.Geometry.Emissive, ScreenTexCoord(), 0).rgb;
	float	AO_sample = textureLod(Texture.Geometry.AO, ScreenTexCoord(), 0).x;
	_cdiff = CDiff_sample.rgb;
	_f0 = F0_sample.rgb;
	_alpha = F0_sample.a;
	_opacity = CDiff_sample.a;
	_normal = Normal_sample;
	_depth = Depth_sample;
	_emissive = Emissive_sample;
	_ao = AO_sample;
	//SetWorldNormal(decodeNormal(EncodedNormal()));
	SetWorldPosition(WorldPosition(ScreenTexCoord()));
}
)""