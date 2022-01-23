#version 330 core

layout(location = 1) out vec4 outColor;
layout(location = 0) out float outZ;


in vec3 vNormal;
in vec3 color;
in vec3 pos;
in vec3 norm;
in vec2 texCoord;

uniform sampler2D colorTex;
uniform sampler2D specularTex;
uniform sampler2D emiTex;
uniform sampler2D normalTex;

//Propiedades del objeto
vec3 Ka;
vec3 Kd;
vec3 Ks;
vec3 N;
float alpha = 500.0;
vec3 Ke;
vec3 Pp;

vec3 Ka2;
vec3 Kd2;
vec3 Ks2;
vec3 Ke2;
float alpha2 = 500.0;
vec3 N2;
vec3 Pp2;

//Fog
vec3 fogColor = vec3(1.0);
float fog = 0.0;
float df = 0.005;
int m = 5;

//Propiedades de la luz
vec3 Ia = vec3 (0.0);
vec3 Id = vec3 (1000.0);
vec3 Is = vec3 (1000.0);
vec3 lpos = vec3 (0.0, 0.0, 5.0); 

vec3 Ia2 = vec3(0.0);
vec3 Id2 = vec3(0.5);
vec3 Is2 = vec3(0.5);
vec3 lpos2 = vec3(50.0,50.0,50.0);

vec3 shade();

void main()
{
	Ka = texture(colorTex, texCoord).rgb;
	Kd = texture(colorTex, texCoord).rgb;
	Ke = texture(emiTex, texCoord).rgb;
	Ks = texture(specularTex, texCoord).rgb;

	Ka2 = texture(colorTex, texCoord).rgb;
	Kd2 = texture(colorTex, texCoord).rgb;
	Ks2 = texture(specularTex, texCoord).rgb;
	Ke2 = texture(emiTex, texCoord).rgb;

	N = texture(normalTex, texCoord).rgb;
	N = normalize(N * 2.0 - 1.0);	
	
	Pp = pos;
	Pp2 = pos;

	outColor = vec4(shade(), 1.0);   
	outZ = pos.z;
	//gl_FragDepth = outZ;
}

vec3 shade()
{
	vec3 c = vec3(0);

	//Ambiental
	c += Ia*Ka;

	//Difusa
	vec3 L = normalize(lpos - Pp);
	c += Id * Kd * max(dot(N, L), 0) * (pow(1/(length(Pp - lpos)+1), 2));

	//Especular
	vec3 V = normalize(-pos);
	vec3 H = normalize(V+L);
	c+= Is * Ks * pow (max(dot(H, N),0), alpha) * (pow(1/(length(Pp - lpos)+1), 2));

	c+= Ke;

	vec3 c2 = vec3(0);

	//Ambiental
	c2 += Ia2*Ka2;

	//Difusa
	vec3 L2 = normalize(lpos2);
	c2 += Id2 * Kd2 * max(dot(N, L2), 0);

	//Especular
	vec3 V2 = normalize(-Pp2);
	vec3 H2 = normalize(V2+L2);
	c2 += Is2 * Ks2 * pow (max(dot(H2, N),0), alpha2);

	c2 += Ke2;

	return c + c2;
}
