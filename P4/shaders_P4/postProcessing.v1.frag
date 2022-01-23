#version 330 core

in vec2 texCoord;
uniform sampler2D colorTex;
uniform sampler2D zTex;
out vec4 outColor;

in float focalDistance;				// = -25.0;
in float maxDistanceFactor;			// = 1.0/5.0;

in float mask[9];

float near = 0.1; 
float far  = 5000.0; 

#define MASK_SIZE 9u

const vec2 texIdx[MASK_SIZE] = vec2[](
vec2(-1.0,1.0), vec2(0.0,1.0), vec2(1.0,1.0),
vec2(-1.0,0.0), vec2(0.0,0.0), vec2(1.0,0.0),
vec2(-1.0,-1.0), vec2(0.0,-1.0), vec2(1.0,-1.0));


void main()
{
	//Sería más rápido utilizar una variable uniform el tamaño de la textura.
	vec2 ts = vec2(1.0) / vec2 (textureSize (colorTex,0));

	float z = (2.0 * near * far) / (far + near - (gl_FragDepth * 2.0 - 1.0) * (far - near));	

	float dof = abs(texture(zTex,texCoord).x - focalDistance) * maxDistanceFactor;
	
	dof = clamp (dof, 0.0, 1.0);
	dof *= dof;
	vec4 color = vec4 (0.0);
	for (uint i = 0u; i < MASK_SIZE; i++)
	{
		vec2 iidx = texCoord + ts * texIdx[i] * dof;
		color += texture(colorTex, iidx,0.0) * mask[i];
	}
	outColor = color;
}
