#version 330 core

layout (location = 0) in vec3 inPos;	

uniform float focalDist;
uniform float distFactor;

uniform float[9] msk;
uniform uint msksz; 

out float focalDistance;
out float maxDistanceFactor;

out float[9] mask;

out vec2 texCoord;

void main()
{
	focalDistance = focalDist;
	maxDistanceFactor = distFactor;

	mask = msk;

	texCoord = 0.5*inPos.xy+vec2(0.5);
	gl_Position = vec4(inPos,1.0);
}
