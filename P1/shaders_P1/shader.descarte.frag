#version 330 core

out vec4 outColor;
in vec2 texCoord;

uniform sampler2D colorTex;

void main()
{
	/*descarte por condiciones de contorno*/
	float rs = pow((texCoord.x - 0.5f), 2) + pow((texCoord.y - 0.5f), 2);
	if(rs < 0.2f)
		discard;
	outColor = vec4(texCoord, vec2(1));
	

	/*descarte por color
	if(outColor.rgb == 0.0f)    
		discard;
	outColor = texture(colorTex, texCoord);
	*/
}