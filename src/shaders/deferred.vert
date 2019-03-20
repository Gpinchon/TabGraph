R""(
layout(location = 0) in vec2	in_Position;

struct t_Vert {
	vec2	UV;
	vec3	CubeUV;
	vec3	Position;
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


/* uniform mat4	in_InvProjMatrix;
uniform mat4	in_InvViewMatrix; */
uniform t_Camera	Camera;

out vec2		frag_UV;
out vec3		frag_Cube_UV;

t_Vert			Vert;

void	FillIn()
{
	Vert.Position = vec3(in_Position, 0);
	Vert.UV = vec2(in_Position.x == -1 ? 0 : 1, in_Position.y == -1 ? 0 : 1);
	Vert.CubeUV = -mat3(Camera.InvMatrix.View) * (Camera.InvMatrix.Projection * vec4(Vert.Position, 1)).xyz;
}

void	FillOut()
{
	gl_Position = vec4(Vert.Position, 1);
	frag_UV = Vert.UV;
	frag_Cube_UV = Vert.CubeUV;
}

void	ApplyTechnique();

void main()
{
	FillIn();
	ApplyTechnique();
	FillOut();
}

)""