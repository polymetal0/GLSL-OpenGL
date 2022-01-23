#version 330 core


layout(location=0) in vec3 inPos;	
layout(location=1) in vec3 inColor;
layout(location=2) in vec3 inNormal;
layout(location=3) in vec2 inTexCoord;

in vec3 inTangent;
in vec3 inBitangent;
uniform mat4 modelViewProj;
uniform mat4 normal;
uniform mat4 modelView;

out vec3 vNormal;
out vec3 vPos;
out vec3 vColor;
out vec2 vTexCoord;
out mat3 TBN;

vec3 vTangent;
vec3 vBitangent;


void main()
{
	vNormal = normalize(vec3(normal * vec4 (inNormal, 0)));
	vTangent = normalize(vec3(normal * vec4(inTangent, 0.0)));
	vBitangent = cross(vNormal, vTangent);
	TBN = mat3(vTangent, vBitangent, vNormal);	

	vPos = vec3(modelView * vec4 (inPos, 1));
	vColor = inColor;
	vTexCoord = inTexCoord;
	

	gl_Position =  modelViewProj * vec4 (inPos,1.0);
}
