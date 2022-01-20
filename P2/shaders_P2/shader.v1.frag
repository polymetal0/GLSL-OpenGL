#version 330 core

out vec4 outColor;
in vec3 color;


//Propiedades de las funtes de luz
//uniforms
vec3 Ia = vec3(0.1);
vec3 Il = vec3(1.0);
vec3 lpos = vec3(0);//Coord cámara

//Propiedades del objeto
vec3 Ka;
vec3 Kd;
vec3 Ks;
float n;
vec3 N;
vec3 pos;

vec3 shade()
{
	vec3 c = vec3(0);

	//Ambiental
	c += Ia*Ka;

	//Difusa
	vec3 L = normalize(lpos - pos);
	c+= Il * Kd * max(dot(N, L), 0);

	//Especular
	vec3 V = normalize(-pos);
	vec3 R = normalize(reflect(-L,N));
	c+= Il * Ks * pow (max(dot(V, R),0), n);
	//vec3 H = normalize(V+L);
	//c+= Il * Ks * pow (max(dot(H, N),0), n);

	return c;
}

in vec3 vNormal;
in vec3 vPos;
in vec3 vColor;

void main()
{
	Ka = vColor;
	Kd = Ka;
	Ks = vec3(1);
	n = 200;

	N = normalize(vNormal);
	pos = vPos;

	outColor = vec4(shade(), 1.0);   
}
