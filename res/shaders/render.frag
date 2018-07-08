#version 410
#pragma optionNV (unroll all)
#define	KERNEL_SIZE 5

uniform sampler2D	in_Texture_Color;
uniform sampler2D	in_Texture_Bright;
uniform sampler2D	in_Texture_Normal;
uniform sampler2D	in_Texture_Position;
uniform sampler2D	in_Texture_Depth;
uniform samplerCube	in_Texture_Env;
uniform samplerCube	in_Texture_Env_Spec;

in vec2				frag_UV;
in vec3				frag_Cube_UV;
in float			frag_CenterDepth;

out vec4			out_Color;

vec2 poissonDisk[] = vec2[64](
	vec2(-0.613392, 0.617481),
	vec2(0.170019, -0.040254),
	vec2(-0.299417, 0.791925),
	vec2(0.645680, 0.493210),
	vec2(-0.651784, 0.717887),
	vec2(0.421003, 0.027070),
	vec2(-0.817194, -0.271096),
	vec2(-0.705374, -0.668203),
	vec2(0.977050, -0.108615),
	vec2(0.063326, 0.142369),
	vec2(0.203528, 0.214331),
	vec2(-0.667531, 0.326090),
	vec2(-0.098422, -0.295755),
	vec2(-0.885922, 0.215369),
	vec2(0.566637, 0.605213),
	vec2(0.039766, -0.396100),
	vec2(0.751946, 0.453352),
	vec2(0.078707, -0.715323),
	vec2(-0.075838, -0.529344),
	vec2(0.724479, -0.580798),
	vec2(0.222999, -0.215125),
	vec2(-0.467574, -0.405438),
	vec2(-0.248268, -0.814753),
	vec2(0.354411, -0.887570),
	vec2(0.175817, 0.382366),
	vec2(0.487472, -0.063082),
	vec2(-0.084078, 0.898312),
	vec2(0.488876, -0.783441),
	vec2(0.470016, 0.217933),
	vec2(-0.696890, -0.549791),
	vec2(-0.149693, 0.605762),
	vec2(0.034211, 0.979980),
	vec2(0.503098, -0.308878),
	vec2(-0.016205, -0.872921),
	vec2(0.385784, -0.393902),
	vec2(-0.146886, -0.859249),
	vec2(0.643361, 0.164098),
	vec2(0.634388, -0.049471),
	vec2(-0.688894, 0.007843),
	vec2(0.464034, -0.188818),
	vec2(-0.440840, 0.137486),
	vec2(0.364483, 0.511704),
	vec2(0.034028, 0.325968),
	vec2(0.099094, -0.308023),
	vec2(0.693960, -0.366253),
	vec2(0.678884, -0.204688),
	vec2(0.001801, 0.780328),
	vec2(0.145177, -0.898984),
	vec2(0.062655, -0.611866),
	vec2(0.315226, -0.604297),
	vec2(-0.780145, 0.486251),
	vec2(-0.371868, 0.882138),
	vec2(0.200476, 0.494430),
	vec2(-0.494552, -0.711051),
	vec2(0.612476, 0.705252),
	vec2(-0.578845, -0.768792),
	vec2(-0.772454, -0.090976),
	vec2(0.504440, 0.372295),
	vec2(0.155736, 0.065157),
	vec2(0.391522, 0.849605),
	vec2(-0.620106, -0.328104),
	vec2(0.789239, -0.419965),
	vec2(-0.545396, 0.538133),
	vec2(-0.178564, -0.596057));

uniform float		gaussian_kernel[] = float[KERNEL_SIZE](
	0.06136, 0.24477, 0.38774, 0.24477, 0.06136
);

void main()
{
	vec3	normal = normalize(texture(in_Texture_Normal, frag_UV).xyz);
	vec3	position = texture(in_Texture_Position, frag_UV).xyz;
	float	depth = texture(in_Texture_Depth, frag_UV).r;
	float	dof = min(5, 10.f * abs(frag_CenterDepth - depth));
	vec4	env = textureLod(in_Texture_Env, frag_Cube_UV, 10.f * abs(frag_CenterDepth - 1));
	vec4	color = textureLod(in_Texture_Color, frag_UV, dof);
	float	sampledist = (textureSize(in_Texture_Position, 0).x + textureSize(in_Texture_Position, 0).y) / 2.f / 50.f;
	vec3	finalColor = vec3(0);
	float	occlusion = 0.f;
	if (depth != 1)
	{
		for (int i = 0; i < 64; ++i)
		{
			vec2	index = poissonDisk[i];
			vec2	sampleUV = frag_UV + index * sampledist / textureSize(in_Texture_Position, 0);
			vec3	samplePosition = texture(in_Texture_Position, sampleUV).xyz;
			if (texture(in_Texture_Depth, sampleUV).r <= depth)
			{
				vec3	V = samplePosition - position;
				float	D = length(V);
				float	bias = 0.25;
				float	factor = max(0, dot(normal, normalize(V)));
				float	angle = max(0, factor - bias);
				occlusion += (angle * (1.f / (1.f + D)));
			}
		}
		occlusion /= 64.f;
		/* for (int i = 0; i < KERNEL_SIZE; i++) 
		{
			for (int j = 0; j < KERNEL_SIZE; j++) 
			{
				float	weight = gaussian_kernel[i] * gaussian_kernel[j];
				vec2	index = vec2(float(i - KERNEL_SIZE / 2.f), float(j - KERNEL_SIZE / 2.f));
				vec2	sampleUV = frag_UV + index * sampledist / textureSize(in_Texture_Position, 0);
				vec3	samplePosition = texture(in_Texture_Position, sampleUV).xyz;
				if (texture(in_Texture_Depth, sampleUV).r <= depth)
				{
					vec3	V = samplePosition - position;
					float	D = length(V);
					float	bias = 0.25;
					float	factor = max(0, dot(normal, normalize(V)));
					float	angle = max(0, factor - bias);
					occlusion += (angle * (1.f / (1.f + D))) * weight;
				}
			}
		} */
	}
	occlusion *= (1 - dof / 5.f);
	finalColor = texture(in_Texture_Bright, frag_UV).rgb + color.rgb * color.a * (1 - occlusion);
	out_Color = (env) * max(0, 1 - color.a);
	out_Color += vec4(finalColor, 1);
	//out_Color.rgb = pow(out_Color.rgb, vec3(1 / 2.2));
	//out_Color = vec4(vec3(1 - occlusion), 1);
}
