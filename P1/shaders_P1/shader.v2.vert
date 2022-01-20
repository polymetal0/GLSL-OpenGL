#version 330 core

in vec3 inPos;
in vec3 inColor;	

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 modelViewProj;

out vec4 vcolor;

void main()
{
	vcolor = inColor.rgbb;
	gl_Position =  modelViewProj * vec4(inPos, 1.0);
}
