#version 330 core

in vec3 inPos;	
in vec3 inColor;
in vec3 inNormal;
in vec2 inTexCoord;
uniform mat4 modelViewProj;
uniform mat4 normal;
uniform mat4 modelView;


out vec3 vNormal;
out vec3 vPos;
out vec3 vColor;
out vec2 vTexCoord;


void main()
{
	vNormal = normalize(vec3(normal * vec4 (inNormal, 0)));
	vPos = vec3(modelView * vec4 (inPos, 1));
	vColor = inColor;
	vTexCoord = inTexCoord;
	

	gl_Position =  modelViewProj * vec4 (inPos,1.0);
}
