#version 330 core

out vec4 outColor;

uniform sampler2D colorTex;
uniform sampler2D specularTex;
uniform sampler2D emiTex;
uniform sampler2D normalTex;

in vec3 color;

in vec3 LightPos;
in vec3 LightIntensity;

//Propiedades de las funtes de luz
//uniforms
vec3 Ia = vec3(0.0);
vec3 Il = LightIntensity;		//vec3(1000.0);
vec3 Pl = LightPos;				//vec3(0,1.0,0);//Coord cámara

vec3 Ia2 = vec3(0.0);
vec3 Il2 = vec3(1.0);
vec3 Pl2 = vec3(50.0,50.0,50.0);
//Objetos
vec3 Ka;
vec3 Kd;
vec3 Ks;
vec3 Ke;
float n;
vec3 N;
vec3 Pp;// = glm::vec3(0.0, 1.0, 0.0);

vec3 Ka2;
vec3 Kd2;
vec3 Ks2;
vec3 Ke2;
float n2;
vec3 N2;
vec3 Pp2;

//Fog
vec3 fogColor = vec3(1.0);//vec3(0.5);
float fog = 0.0;
float df = 0.005;
int m = 5;

vec3 shade()
{
	vec3 c = vec3(0);

	//Ambiental
	c += Ia*Ka;

	//Difusa
	vec3 L = normalize(Pl - Pp);
	c+= Il * Kd * max(dot(N, L), 0) * (pow(1/(length(Pp - Pl)+1), 2));

	//Especular
	vec3 V = normalize(-Pp);
	//vec3 R = normalize(reflect(-L,N));
	//c+= Il * Ks * pow (max(dot(V, R),0), n);
	vec3 H = normalize(V+L);
	c+= Il * Ks * pow (max(dot(H, N),0), n) * (pow(1/(length(Pp - Pl)+1), 2));

	c+= Ke;// * (pow(1/(length(Pp - Pl)+1), 2));

	vec3 c2 = vec3(0);

	//Ambiental
	c2 += Ia2*Ka2;

	//Difusa
	vec3 L2 = normalize(Pl2);
	c2 += Il2 * Kd2 * max(dot(N, L2), 0);

	//Especular
	vec3 V2 = normalize(-Pp2);
	//vec3 R = normalize(reflect(-L,N));
	//c+= Il * Ks * pow (max(dot(V, R),0), n);
	vec3 H2 = normalize(V2+L2);
	c2 += Il2 * Ks2 * pow (max(dot(H2, N),0), n2);

	c2 += Ke2;

	return c + c2;
}

in vec3 vNormal;
in vec3 vPos;
in vec3 vColor;
in vec2 vTexCoord;
//in vec3 vTangent;
//in vec3 vBitangent;
//in mat3 TBN;

void main()
{
	Ka = texture(colorTex, vTexCoord).rgb;
	Kd = Ka;
	Ks = texture(specularTex, vTexCoord).rgb;
	n = 200;
	Ke = texture(emiTex, vTexCoord).rgb;

	Ka2 = texture(colorTex, vTexCoord).rgb;
	Kd2 = Ka2;
	Ks2 = texture(specularTex, vTexCoord).rgb;
	n2 = 200;
	Ke2 = texture(emiTex, vTexCoord).rgb;

	N = texture(normalTex, vTexCoord).rgb;
	N = normalize(N * 2.0 - 1.0);	

	Pp = vPos;
	Pp2 = vPos;

	fog = exp(-pow((df * Pp.z), 2));
	fog = clamp(fog, 0.0, 1.0);
	outColor = fog * vec4(shade(), 1.0) + (1 - fog) * vec4(fogColor, 1.0);
	//outColor = vec4(shade(), 1.0);   
}
