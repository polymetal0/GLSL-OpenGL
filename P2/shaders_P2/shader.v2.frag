#version 330 core

out vec4 outColor;
uniform sampler2D colorTex;
uniform sampler2D specularTex;
uniform sampler2D emiTex;
in vec3 color;


//Propiedades de las funtes de luz
//uniforms
vec3 Ia = vec3(0.1);
vec3 Il = vec3(1.0);
vec3 Pl = vec3(0,-1.0,0);//Coord cámara

//Objeto
vec3 Ka;
vec3 Kd;
vec3 Ks;
vec3 Ke;
float n;
vec3 N;
vec3 Pp;

vec3 shade()
{
	vec3 c = vec3(0);

	//Ambiental
	c += Ia*Ka;

	//Difusa
	vec3 L = normalize(Pl - Pp);
	c+= Il * Kd * max(dot(N, L), 0);

	//Especular
	vec3 V = normalize(-Pp);
	//vec3 R = normalize(reflect(-L,N));
	//c+= Il * Ks * pow (max(dot(V, R),0), n);
	vec3 H = normalize(V+L);
	c+= Il * Ks * pow (max(dot(H, N),0), n);

	c+= Ke;

	return c;
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

	N = normalize(vNormal);
	Pp = vPos;

	outColor = vec4(shade(), 1.0);   
}
