layout(location = 0) in vec3	in_Position;
layout(location = 2) in vec2	in_Texcoord;
uniform mat4					in_Transform;

out vec2	frag_Texcoord;
void main()
{
	frag_Texcoord = in_Texcoord;
	gl_Position = in_Transform * vec4(in_Position, 1);
}
