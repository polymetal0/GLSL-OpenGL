#version 330 core


layout(location=0) in vec3 inPos;	
layout(location=1) in vec3 inColor;
layout(location=2) in vec3 inNormal;
layout(location=3) in vec2 inTexCoord;

in vec3 inTangent;
in vec3 inBitangent;
uniform mat4 modelViewProj;
uniform mat4 normal;
uniform mat4 view;
uniform mat4 modelView;
uniform vec3 lightpos;

out vec3 LightPos;


out vec3 vNormal;
out vec3 vPos;
out vec3 vColor;
out vec2 vTexCoord;
out vec3 vTangent;
out vec3 vBitangent;
out mat3 TBN;


void main()
{
	vNormal = normalize(vec3(normal * vec4 (inNormal, 0)));
	vTangent = normalize(vec3(normal * vec4(inTangent, 0.0)));
	vBitangent = cross(vNormal, vTangent);//, vNormal);
	TBN = mat3(vTangent, vBitangent, vNormal);	

	vPos = vec3(modelView * vec4 (inPos, 1));
	vColor = inColor;
	vTexCoord = inTexCoord;
	
	LightPos = vec3(view * vec4(vec3(0.0,6.0,6.0),1.0));

	gl_Position =  modelViewProj * vec4 (inPos,1.0);
}
