R""(
layout(location = 0) out vec4	out_0;
layout(location = 1) out vec4	out_1;
layout(location = 2) out vec4	out_2;

struct t_Environment {
	samplerCube	Diffuse;
	samplerCube	Irradiance;
};

in VertexData {
	vec2	TexCoord;
	vec3	CubeTexCoord;
} Frag;

in vec2	frag_UV;

uniform t_Environment Environment;

void OutputEnv() {
	out_0 = texture(Environment.Diffuse, Frag.CubeTexCoord, 0);
	out_1 = vec4(0);
	out_2 = vec4(0);
}

)""