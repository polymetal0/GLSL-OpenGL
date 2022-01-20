#version 330 core

in vec3 inPos;
in vec3 inColor;
in vec3 inNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 modelViewProj;
uniform mat4 normal; //modelView inversa traspuesta

out vec4 vcolor;

void main()
{
	vcolor = normal * vec4(inNormal.xyz,0);
	gl_Position =  modelViewProj * vec4(inPos, 1.0);
}
