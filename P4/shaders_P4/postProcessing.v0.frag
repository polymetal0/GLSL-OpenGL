#version 330 core

in vec2 texCoord;
uniform sampler2D colorTex;
out vec4 color;

void main()
{

	//C�digo del Shader
	//NOTA: Ser�a m�s adecuado usar texelFetch.
	//NOTA: No lo hacemos porque simplifica el paso 5
	color = vec4(textureLod(colorTex, texCoord,0).xyz, 0.6);
	//color = vec4(texCoord,vec2(1.0));
}