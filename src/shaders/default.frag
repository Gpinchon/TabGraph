#version 330 core

in	vec3 frag_Normal;
out vec4 out_color;

void main()
{
  out_color = vec4(frag_Normal, 1);
  //out_color = vec4(1, 0, 0, 1);
}
