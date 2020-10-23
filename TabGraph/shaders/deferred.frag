R""(
#define M_PI	3.1415926535897932384626433832795
#define PI		M_PI
#define EPSILON	0.0001

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
	sampler2D		Ior;
	sampler2D		AO;
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


#ifdef LIGHTSHADER
	struct t_BackTextures {
		sampler2D	Color;
		sampler2D	Emissive;
	};
	
	struct t_Textures {
		sampler2D			BRDF;
		t_BackTextures		Back;
		t_GeometryTextures	Geometry;
		t_Environment		Environment;
	};
	
	uniform t_Textures		Texture;
	uniform t_Camera		Camera;
	uniform vec3			Resolution;
	uniform float			Time;
	uniform uint			FrameNumber;
	
	layout(location = 0) out vec4	out_Color;
	layout(location = 1) out vec3	out_Emissive;
	
	float _opacity = 1; 
	#define Opacity() (_opacity)
	#define SetOpacity(opacity) (_opacity = opacity)
	
	vec3 _cdiff = vec3(1);
	#define CDiff() (_cdiff)
	#define SetCDiff(cDiff) (_cdiff = cDiff)
	
	vec3 _f0 = vec3(0.04);
	#define F0() (_f0)
	#define SetF0(f0) (_f0 = f0)
	
	float _alpha = 1;
	#define Alpha() (_alpha)
	#define SetAlpha(alpha) (_alpha = alpha)
	
	vec3 _emissive = vec3(0);
	#define Emissive() (_emissive)
	#define SetEmissive(emissive) (_emissive = emissive)
	
	float _ior = 1;
	#define Ior() (_ior)
	#define SetIor(ior) (_ior = ior)
	
	float _ao = 0;
	#define AO() (_ao)
	#define SetAO(aO) (_ao = aO)
	
	vec2 _normal = vec2(0);
	#define EncodedNormal() (_normal)
	#define SetEncodedNormal(normal) (_normal = normal)

	//#define WorldNormal() (decodeNormal(_normal))
	//#define SetWorldNormal(normal) (_normal = encodeNormal(normal))
	
	#define BackColor() (out_Color)
	#define SetBackColor(backColor) (out_Color = backColor)
	
	#define BackEmissive() (out_Emissive)
	#define SetBackEmissive(backEmissive) (out_Emissive = backEmissive)
#endif //LIGHTSHADER

#ifdef POSTSHADER
	struct t_BackTextures {
		sampler2D	Color;
		sampler2D	Emissive;
	};
	
	struct t_Textures {
		sampler2D			BRDF;
		t_BackTextures		Back;
		t_GeometryTextures	Geometry;
		t_Environment		Environment;
	};
	layout(location = 0) out vec4	out_CDiff; //BRDF CDiff, Transparency
	layout(location = 1) out vec3	out_Emissive;
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

	#define Emissive() (out_Emissive)
	#define SetEmissive(emissive) (out_Emissive = emissive)

	#define Ior() (out_Ior)
	#define SetIor(ior) (out_Ior = ior)

	#define AO() (out_AO)
	#define SetAO(aO) (out_AO = aO)

	#define EncodedNormal() (out_Normal)
	#define SetEncodedNormal(normal) (out_Normal = normal)
#endif //POSTSHADER

float Depth() { return gl_FragDepth; }
float Depth(vec2 uv) { return textureLod(Texture.Geometry.Depth, uv, 0).r; }
#define SetDepth(depth) (gl_FragDepth = depth)

t_Frag	Frag;

vec3	ScreenToWorld(in vec2 uv, in float depth)
{
	vec4	projectedCoord = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	projectedCoord = Camera.InvMatrix.View * Camera.InvMatrix.Projection * projectedCoord;
	return (projectedCoord.xyz / projectedCoord.w);
}

/*bool _worldPositionSet = false;
void SetWorldPosition(in vec3 worldPosition) {
	Frag.WorldPosition = worldPosition;
	_worldPositionSet = true;
}*/

#define SetWorldPosition(worldPosition) (Frag.WorldPosition = worldPosition)

vec3	WorldPosition(in vec2 uv)
{
	return ScreenToWorld(uv, Depth(uv));
}

vec3 WorldPosition()
{
	return Frag.WorldPosition;
}

void SetWorldNormal(in vec3 worldNormal)
{
	Frag.WorldNormal = normalize(worldNormal);
	SetEncodedNormal(encodeNormal(Frag.WorldNormal));
}

vec3 WorldNormal()
{
	return Frag.WorldNormal;
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

#define lequal(a, b) all(lessThanEqual(a, b))

float Luminance(vec3 LinearColor)
{
	return dot(LinearColor, vec3(0.299, 0.587, 0.114));
}

void FillFragmentData(void)
{
	vec4	CDiff_sample = textureLod(Texture.Geometry.CDiff, TexCoord(), 0);
	vec4	F0_sample = textureLod(Texture.Geometry.F0, TexCoord(), 0);
	vec2	Normal_sample = textureLod(Texture.Geometry.Normal, TexCoord(), 0).xy;
	float	Depth_sample = textureLod(Texture.Geometry.Depth, TexCoord(), 0).x;
	vec3	Emissive_sample = textureLod(Texture.Geometry.Emissive, TexCoord(), 0).rgb;
	float	Ior_sample = textureLod(Texture.Geometry.Ior, TexCoord(), 0).x;
	float	AO_sample = textureLod(Texture.Geometry.AO, TexCoord(), 0).x;
	SetCDiff(CDiff_sample.rgb);
	SetF0(F0_sample.rgb);
	SetAlpha(F0_sample.a);
	SetOpacity(CDiff_sample.a);
	SetEncodedNormal(Normal_sample);
	SetDepth(Depth_sample);
	SetEmissive(Emissive_sample);
	SetIor(Ior_sample);
	SetAO(AO_sample);
#ifdef LIGHTSHADER
	vec4 BackColor_sample = textureLod(Texture.Back.Color, TexCoord(), 0);
	vec3 BackEmissive_sample = textureLod(Texture.Back.Emissive, TexCoord(), 0).rgb;
	SetBackColor(BackColor_sample);
	SetBackEmissive(BackEmissive_sample);
#endif //LIGHTSHADER
	SetWorldNormal(decodeNormal(EncodedNormal()));
	SetWorldPosition(WorldPosition(TexCoord()));
}

#define BRDF(NdV) (texture(Texture.BRDF, vec2(NdV, Alpha())).xy)

/*void	FillFrag()
{
	SetDepth(Depth(TexCoord()));
	SetWorldPosition(WorldPosition(TexCoord()));
	WorldNormal() = decodeNormal(texture(Texture.Normal, TexCoord()).xy);
	Frag.Ior = texture(Texture.Ior, TexCoord()).x;
	vec4	CDiff_sample = texture(Texture.CDiff, TexCoord());
	vec4	F0_sample = texture(Texture.F0, TexCoord());
	SetCDiff(CDiff_sample.rgb);
	SetF0(F0_sample.rgb);
	SetAlpha(F0_sample.a);
	SetOpacity(CDiff_sample.a);
	Frag.Emissive = texture(Texture.Emissive, TexCoord()).rgb;
	Frag.AO = texture(Texture.AO, TexCoord()).r;
#ifdef LIGHTSHADER
	Out.Color = texture(Texture.Back.Color, TexCoord());
	Out.Emissive = texture(Texture.Back.Emissive, TexCoord()).rgb;
#endif
}*/

/*#ifdef POSTSHADER
void	FillOut(in vec3 OriginalPosition)
{
	gl_FragDepth = Frag.Depth;
	bvec3	positionsEqual = notEqual(Frag.Position, OriginalPosition);
	if (positionsEqual.x || positionsEqual.y || positionsEqual.z)
	{
		vec4	NDC = Camera.Matrix.Projection * Camera.Matrix.View * vec4(Frag.Position, 1.0);
		gl_FragDepth = NDC.z / NDC.w * 0.5 + 0.5;
	}
	out_CDiff = vec4(Frag.BRDF.CDiff, Frag.Opacity);
	out_F0.rgb = Frag.BRDF.F0;
	out_F0.a = Frag.BRDF.Alpha;
	out_Emissive = Frag.Emissive;
	out_Ior = Frag.Ior;
	out_AO = Frag.AO;
	out_Normal = encodeNormal(normalize(WorldNormal()));
}
#endif*/

/*#ifdef LIGHTSHADER
void	FillOut(in vec3 OriginalPosition)
{
	if (any(notEqual(Frag.Position, OriginalPosition)))
	{
		vec4	NDC = Camera.Matrix.Projection * Camera.Matrix.View * vec4(Frag.Position, 1.0);
		gl_FragDepth = NDC.z / NDC.w * 0.5 + 0.5;
	}
	out_Color = Out.Color;
	out_Emissive = Out.Emissive;
}
#endif*/



/*void	ApplyTechnique();

void main()
{
	FillFrag();
	vec3	OriginalPosition = Frag.Position;
	ApplyTechnique();
	FillOut(OriginalPosition);
}*/

)""