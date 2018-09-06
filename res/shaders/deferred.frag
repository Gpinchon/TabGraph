#pragma optionNV (unroll all)
#define M_PI 3.1415926535897932384626433832795

precision lowp float;
precision lowp int;
precision lowp sampler2D;
precision lowp samplerCube;

struct t_Environment {
	samplerCube	Diffuse;
	samplerCube	Irradiance;
};

struct t_Material {
	vec4		Albedo;
	vec3		Specular;
	vec3		Emitting;
	float		Roughness;
	float		Metallic;
	float		Ior;
	float		AO;
};

struct t_Frag {
#ifdef LIGHTSHADER
	vec3		BackColor;
	vec3		BackEmitting;
#endif
	float		Depth;
	vec2		UV;
	vec3		CubeUV;
	vec3		Position;
	vec3		Normal;
	t_Material	Material;
};

#ifdef LIGHTSHADER
struct t_Out {
	vec3		Color;
	vec3		Emitting;
};
#endif

uniform sampler2D	in_Texture_Albedo;
uniform sampler2D	in_Texture_Fresnel;
uniform sampler2D	in_Texture_Emitting;
uniform sampler2D	in_Texture_Material_Values;
uniform sampler2D	in_Texture_AO;
uniform sampler2D	in_Texture_Normal;
uniform sampler2D	in_Texture_Depth;
uniform sampler2D	in_Texture_BRDF;

#ifdef LIGHTSHADER
uniform sampler2D	in_Back_Color;
uniform sampler2D	in_Back_Bright;
#endif

uniform vec3		in_CamPos;
uniform mat4		in_ViewMatrix;
uniform mat4		in_ProjViewMatrix;
uniform mat4		in_InvProjViewMatrix;
uniform mat4		in_InvViewMatrix;
uniform mat4		in_InvProjMatrix;

uniform t_Environment	Environment;

in vec2				frag_UV;
in vec3				frag_Cube_UV;

#ifdef POSTSHADER
layout(location = 0) out vec4	out_Albedo;
layout(location = 1) out vec4	out_Emitting;
layout(location = 2) out vec4	out_Fresnel;
layout(location = 3) out vec4	out_Material_Values; //Roughness, Metallic, Ior
layout(location = 4) out vec4	out_AO;
layout(location = 5) out vec4	out_Normal;
#endif

#ifdef LIGHTSHADER
layout(location = 0) out vec4	out_Color;
layout(location = 1) out vec4	out_Emitting;
#endif

t_Frag	Frag;

#ifdef LIGHTSHADER
t_Out	Out;
#endif

vec3	Position(vec2 UV)
{
	float	linearDepth = texture(in_Texture_Depth, UV).r * 2.0 - 1.0;
	vec2	coord = UV * 2.0 - 1.0;
	vec4	projectedCoord = vec4(coord, linearDepth, 1.0);
	projectedCoord = in_InvProjViewMatrix * projectedCoord;
	return (projectedCoord.xyz / projectedCoord.w);
}

vec3	Position()
{
	float	linearDepth = Frag.Depth * 2.0 - 1.0;
	vec2	coord = frag_UV * 2.0 - 1.0;
	vec4	projectedCoord = vec4(coord, linearDepth, 1.0);
	projectedCoord = in_InvProjViewMatrix * projectedCoord;
	return (projectedCoord.xyz / projectedCoord.w);
}

vec3	OriginalPosition;

void	FillFrag()
{
	Frag.UV = frag_UV;
	Frag.CubeUV = frag_Cube_UV;
	Frag.Depth = gl_FragDepth = texture(in_Texture_Depth, frag_UV).r;
	OriginalPosition = Frag.Position = Position();
	Frag.Normal = texture(in_Texture_Normal, frag_UV).xyz;
	Frag.Material.Albedo = texture(in_Texture_Albedo, frag_UV);
	Frag.Material.Specular = texture(in_Texture_Fresnel, frag_UV).xyz;
	Frag.Material.Emitting = texture(in_Texture_Emitting, frag_UV).xyz;
	vec3	Material_Values = texture(in_Texture_Material_Values, frag_UV).xyz;
	Frag.Material.Roughness = Material_Values.x;
	Frag.Material.Metallic = Material_Values.y;
	Frag.Material.Ior = Material_Values.z;
	Frag.Material.AO = texture(in_Texture_AO, frag_UV).r;
	#ifdef LIGHTSHADER
	Frag.BackColor = 
	Out.Color = vec3(0);
	Out.Emitting = vec3(0);
	#endif
}

#ifdef POSTSHADER
void	FillOut()
{
	gl_FragDepth = Frag.Depth;
	bvec3	positionsEqual = notEqual(Frag.Position, OriginalPosition);
	if (positionsEqual.x || positionsEqual.y || positionsEqual.z)
		gl_FragDepth = ((in_ProjViewMatrix * vec4(Frag.Position, 1.0)).z + 1.0) / 2.0;
	out_Albedo = Frag.Material.Albedo;
	out_Fresnel = vec4(Frag.Material.Specular, 1);
	out_Emitting = vec4(Frag.Material.Emitting, 1);
	out_Material_Values = vec4(Frag.Material.Roughness, Frag.Material.Metallic, Frag.Material.Ior, 1);
	out_AO = vec4(Frag.Material.AO, 0, 0, 1);
	out_Normal = vec4(Frag.Normal, 1);
}
#endif

#ifdef LIGHTSHADER
void	FillOut()
{
	out_Color = vec4(Out.Color, 1);
	out_Emitting = vec4(Out.Emitting, 1);
	gl_FragDepth = Frag.Depth;
}
#endif

vec2	BRDF(in float NdV, in float Roughness)
{
	return (texture(in_Texture_BRDF, vec2(NdV, Frag.Material.Roughness)).xy);
}

vec4	sampleLod(in samplerCube texture, in vec3 uv, in float value)
{
	return textureLod(texture, uv, value * textureQueryLevels(texture));
}

vec4	sampleLod(in sampler2D texture, in vec2 uv, in float value)
{
	return textureLod(texture, uv, value * textureQueryLevels(texture));
}

float	map(in float value, in float low1, in float high1, in float low2, in float high2)
{
	return (low2 + (value - low1) * (high2 - low2) / (high1 - low1));
}

void	ApplyTechnique();

void main()
{
	FillFrag();
	ApplyTechnique();
	FillOut();
}

