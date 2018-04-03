#version 410

layout(location = 0) in vec2	in_Position;

out vec2	frag_UV;

void main()
{
	frag_UV = vec2(in_Position.x == -1 ? 0 : 1, in_Position.y == -1 ? 0 : 1);
	gl_Position = vec4(in_Position, 0, 1);
}
