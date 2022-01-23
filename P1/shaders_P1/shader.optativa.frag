#version 330 core

out vec4 outColor;
in vec2 texCoord;

uniform sampler2D colorTex;


void main()
{
	//pintar un cuadrado en el centro
	float sides = texCoord.x;
	float topBottom = texCoord.y;

	if(sides > 0.25f && sides < 0.75f && topBottom > 0.25f && topBottom < 0.75f)
		outColor = vec4(0.0f, 0.0f, 1.0f , 0.1f);
	else
		outColor = vec4(texCoord, vec2(1));

}