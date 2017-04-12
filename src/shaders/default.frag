#version 330 core

uniform vec3	in_Albedo;
in	vec3		frag_Normal;
out vec4		out_Color;

void main()
{
	out_Color = vec4(in_Albedo, 1);
  //out_Color = vec4(frag_Normal, 1);
  //out_Color = vec4(1, 0, 0, 1);
}
