#version 330 core

out vec4 outColor;
uniform sampler2D colorTex;
uniform sampler2D specularTex;
uniform sampler2D emiTex;
uniform sampler2D normalTex;
in vec3 color;

//Propiedades de las funtes de luz
//uniforms
vec3 Ia = vec3(0.0);
vec3 Il = vec3(1.0);
vec3 Pl = vec3(50.0,50.0,50.0);

vec3 Ia2 = vec3(0.0);
vec3 Il2 = vec3(10.0);
vec3 Pl2 = vec3(0.0,0.0,-2.0);
float angle = 3.14159/6;
float cutoff = 3.14159/4;

//Objeto
vec3 Ka;
vec3 Kd;
vec3 Ks;
vec3 Ke;
float n;
vec3 N;
vec3 Pp;

//Objeto
vec3 Ka2;
vec3 Kd2;
vec3 Ks2;
vec3 Ke2;
float n2;
vec3 N2;
vec3 Pp2;

//Fog
vec3 fogColor = vec3(1.0);
float fog = 0.0;
float df = 0.005;
int m = 5;

vec3 shade()
{
	vec3 c = vec3(0);

	//Ambiental
	c += Ia*Ka;

	//Difusa
	vec3 L = normalize(Pl);
	c += Il * Kd * max(dot(N, L), 0);

	//Especular
	vec3 V = normalize(-Pp);
	vec3 H = normalize(V+L);
	c += Il * Ks * pow (max(dot(H, N),0), n);

	c += Ke;


	vec3 c2 = vec3(0);

	//Ambiental
	c2 += Ia2 * Ka2;

	//Difusa
	vec3 L2 = normalize(Pl2 - Pp2);
	float _LD = length(dot(-(Pl2 - Pp2), Pp2));
	float fdir = 0.0;
	if(cos(angle) > _LD)
	{
		float t = clamp(pow((_LD-cos(cutoff)/(cos(angle)-cos(cutoff))), m), 0.0, 1.0);
		fdir = smoothstep(1,0,t);
	}
	else
	{
		fdir = 0.0;
	}
	c2 += Il2 * Kd2 * max(dot(N2, L2), 0) * fdir * (pow(1/(length(Pp2 - Pl2) + 1), 2));

	//Especular
	vec3 V2 = normalize(-Pp2);
	vec3 H2 = normalize(V2+L2);
	c2 += Il2 * Ks2 * pow(max(dot(H2, N2),0), n2) * fdir * (pow(1/(length(Pp2 - Pl2) + 1), 2));

	c2 += Ke2;

	return c + c2;
}

in vec3 vNormal;
in vec3 vPos;
in vec3 vColor;
in vec2 vTexCoord;

void main()
{	
	Ka = texture(colorTex, vTexCoord).rgb;
	Kd = Ka;
	Ks = texture(specularTex, vTexCoord).rgb;
	n = 200;
	Ke = texture(emiTex, vTexCoord).rgb;

	Pp = vPos;
	
	Ka2 = texture(colorTex, vTexCoord).rgb;
	Kd2 = Ka2;
	Ks2 = texture(specularTex, vTexCoord).rgb;
	n2 = 200;
	Ke2 = texture(emiTex, vTexCoord).rgb;

	Pp2 = vPos;

	N = texture(normalTex, vTexCoord).rgb;
	N = normalize(N * 2.0 - 1.0);

	N2 = N;

	fog = exp(-pow((df * Pp.z), 2));
	fog = clamp(fog, 0.0, 1.0);
	outColor = fog * vec4(shade(), 1.0) + (1 - fog) * vec4(fogColor, 1.0);   
}
