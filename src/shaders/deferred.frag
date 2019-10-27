R""(
#define M_PI	3.1415926535897932384626433832795
#define PI		M_PI
#define EPSILON	0.0001

precision lowp float;
precision lowp int;
precision lowp sampler2D;
precision lowp samplerCube;

struct t_Environment {
	samplerCube	Diffuse;
	samplerCube	Irradiance;
};

#ifdef LIGHTSHADER
struct t_BackTextures {
	sampler2D	Color;
	sampler2D	Emitting;
	sampler2D	Normal;
};
#endif //LIGHTSHADER

struct t_Textures {
	sampler2D		Albedo;
	sampler2D		Emitting;
	sampler2D		Specular;
	sampler2D		MaterialValues;
	sampler2D		AO;
	sampler2D		Normal;
	sampler2D		Depth;
	sampler2D		BRDF;
	t_Environment	Environment;
#ifdef LIGHTSHADER
	t_BackTextures	Back;
#endif //LIGHTSHADER
};

struct t_Material {
	vec3		Albedo;
	vec3		Emitting;
	vec3		Specular;
	float		Roughness;
	float		Metallic;
	float		Ior;
	float		Alpha;
	float		AO;
};

struct t_Frag {
	float		Depth;
	vec2		UV;
	vec3		CubeUV;
	vec3		Position;
	vec3		Normal;
	t_Material	Material;
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

#ifdef LIGHTSHADER
struct t_Out {
	vec4		Color;
	vec3		Emitting;
	vec3		Normal;
};

layout(location = 0) out vec4	out_Color;
layout(location = 1) out vec3	out_Emitting;
layout(location = 2) out vec3	out_Normal;
t_Out	Out;
#endif //LIGHTSHADER

uniform t_Textures		Texture;
uniform t_Camera		Camera;
uniform vec3			Resolution;
uniform float			Time;

in vec2				frag_UV;
in vec3				frag_Cube_UV;

#ifdef POSTSHADER
layout(location = 0) out vec4	out_Albedo;
layout(location = 1) out vec3	out_Emitting;
layout(location = 2) out vec3	out_Fresnel;
layout(location = 3) out vec3	out_Material_Values; //Roughness, Metallic, Ior
layout(location = 4) out float	out_AO;
layout(location = 5) out vec3	out_Normal;
#endif //POSTSHADER

t_Frag	Frag;

mat3x3	tbn_matrix()
{
	vec3 Q1 = dFdx(Frag.Position);
	vec3 Q2 = dFdy(Frag.Position);
	vec2 st1 = dFdx(Frag.UV);
	vec2 st2 = dFdy(Frag.UV);
	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = normalize(-Q1*st2.s + Q2*st1.s);
	return(transpose(mat3(T, B, Frag.Normal)));
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

int		textureMaxLod(in samplerCube tex)
{
#ifdef textureQueryLevels
	return textureQueryLevels(tex);
#else
	//Try to guess the max LOD
	ivec2	texRes = textureSize(tex, 0);
	return int(log2(max(texRes.x, texRes.y)));
#endif
}

int		textureMaxLod(in sampler2D tex)
{
#ifdef textureQueryLevels
	return textureQueryLevels(tex);
#else
	//Try to guess the max LOD
	ivec2	texRes = textureSize(tex, 0);
	return int(log2(max(texRes.x, texRes.y)));
#endif
}

vec4	texelFetchLod(in sampler2D tex, in vec2 uv, in float mipLevel)
{
	ivec2	Resolution = textureSize(tex, 0);
	return mix(texelFetch(tex, ivec2(Resolution * uv), int(floor(mipLevel))), texelFetch(tex, ivec2(Resolution * uv), int(ceil(mipLevel))), fract(mipLevel));
}

vec4	texelFetchLod(in sampler2D tex, in vec2 uv, in int mipLevel)
{
	ivec2	Resolution = textureSize(tex, 0);
	return texelFetch(tex, ivec2(Resolution * uv), mipLevel);
}

/* vec4	texelFetchLod(in sampler2D tex, in vec2 uv, in float value)
{
	int		maxLOD = textureMaxLod(tex);
	int		lodValue = maxLOD * value;
	return texelFetchLod(tex, uv, lodValue);
} */

vec4	sampleLod(in samplerCube tex, in vec3 uv, in float value)
{
	float maxLOD = textureMaxLod(tex);
	return textureLod(tex, uv, value * maxLOD);
}

vec4	sampleLod(in sampler2D tex, in vec2 uv, in float value)
{
	float maxLOD = textureMaxLod(tex);
	return textureLod(tex, uv, value * maxLOD);
}

float	map(in float value, in float low1, in float high1, in float low2, in float high2)
{
	return (low2 + (value - low1) * (high2 - low2) / (high1 - low1));
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
	bvec2	eq = equal(v, vec2(0));
	return (eq.x && eq.y);
}

bool	lequal(in vec2 v, in vec2 v1)
{
	bvec2	eq = lessThanEqual(v, v1);
	return (eq.x && eq.y);
}

float Luminance(vec3 LinearColor)
{
	return dot(LinearColor, vec3(0.299, 0.587, 0.114));
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
	//return ClipToScreen(WorldToClip(position));
}

vec3 TangentToWorld(in vec3 vec)
{
	return normalize(vec * tbn_matrix());
}

vec3	TangentToWorld(in vec2 vec)
{
	return TangentToWorld(vec3(vec, 1));
}

vec3	ScreenToWorld(in vec2 UV, in float depth)
{
	vec4	projectedCoord = vec4(UV * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	projectedCoord = Camera.InvMatrix.View * Camera.InvMatrix.Projection * projectedCoord;
	return (projectedCoord.xyz / projectedCoord.w);
}

float	Depth(in vec2 UV)
{
	return texture(Texture.Depth, UV).r;
}

vec3	WorldPosition(in vec2 UV)
{
	return ScreenToWorld(UV, Depth(UV));
}

vec3	WorldPosition()
{
	return ScreenToWorld(Frag.UV, Frag.Depth);
}

void	FillFrag()
{
	Frag.UV = frag_UV;
	Frag.CubeUV = frag_Cube_UV;
	Frag.Depth = gl_FragDepth = texture(Texture.Depth, frag_UV).r;
	Frag.Position = WorldPosition();
	Frag.Normal = texture(Texture.Normal, frag_UV).xyz;
	vec4	albedo_sample = texture(Texture.Albedo, frag_UV);
	Frag.Material.Albedo = albedo_sample.rgb;
	Frag.Material.Alpha = albedo_sample.a;
	Frag.Material.Specular = texture(Texture.Specular, frag_UV).xyz;
	Frag.Material.Emitting = texture(Texture.Emitting, frag_UV).xyz;
	vec3	MaterialValues = texture(Texture.MaterialValues, frag_UV).xyz;
	Frag.Material.Roughness = MaterialValues.x;
	Frag.Material.Metallic = MaterialValues.y;
	Frag.Material.Ior = MaterialValues.z;
	Frag.Material.AO = texture(Texture.AO, frag_UV).r;
#ifdef LIGHTSHADER
	Out.Color = texture(Texture.Back.Color, frag_UV);
	Out.Emitting = texture(Texture.Back.Emitting, frag_UV).rgb;
	Out.Normal = texture(Texture.Normal, frag_UV).rgb;
#endif
}

#ifdef POSTSHADER
void	FillOut(in vec3 OriginalPosition)
{
	gl_FragDepth = Frag.Depth;
	bvec3	positionsEqual = notEqual(Frag.Position, OriginalPosition);
	if (positionsEqual.x || positionsEqual.y || positionsEqual.z)
	{
		vec4	NDC = Camera.Matrix.Projection * Camera.Matrix.View * vec4(Frag.Position, 1.0);
		gl_FragDepth = NDC.z / NDC.w * 0.5 + 0.5;
	}
	out_Albedo = vec4(Frag.Material.Albedo, Frag.Material.Alpha);
	out_Fresnel = Frag.Material.Specular;
	out_Emitting = Frag.Material.Emitting;
	out_Material_Values = vec3(Frag.Material.Roughness, Frag.Material.Metallic, Frag.Material.Ior);
	out_AO = Frag.Material.AO;
	out_Normal = Frag.Normal;
}
#endif

#ifdef LIGHTSHADER
void	FillOut(in vec3 OriginalPosition)
{
	bvec3	positionsEqual = notEqual(Frag.Position, OriginalPosition);
	if (positionsEqual.x || positionsEqual.y || positionsEqual.z)
	{
		vec4	NDC = Camera.Matrix.Projection * Camera.Matrix.View * vec4(Frag.Position, 1.0);
		gl_FragDepth = NDC.z / NDC.w * 0.5 + 0.5;
	}
	out_Color = Out.Color;
	out_Emitting = Out.Emitting;
	out_Normal = Out.Normal;
}
#endif

vec2	BRDF(in float NdV, in float Roughness)
{
	return (texture(Texture.BRDF, vec2(NdV, Frag.Material.Roughness)).xy);
}

void	ApplyTechnique();

void main()
{
	FillFrag();
	vec3	OriginalPosition = Frag.Position;
	ApplyTechnique();
	FillOut(OriginalPosition);
}

)""