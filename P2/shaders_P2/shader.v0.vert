#version 330 core

in vec3 inPos;	
in vec3 inColor;
in vec3 inNormal;
uniform mat4 modelViewProj;
uniform mat4 normal;
uniform mat4 modelView;
out vec3 color;


//Propiedades de las funtes de luz
//uniforms
vec3 Ia = vec3(0.1);
vec3 Il = vec3(1.0);
vec3 Pl = vec3(0);//Coord cámara

//Propiedades del objeto
vec3 Ka;
vec3 Kd;
vec3 Ks;
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
	vec3 R = normalize(reflect(-L,N));
	c+= Il * Ks * pow (max(dot(V, R),0), n);
	vec3 H = normalize(V+L);
	c+= Il * Ks * pow (max(dot(H, N),0), n);


	
	return c;
}


void main()
{
	Ka = vec3(1,0,0);
	Kd = Ka;
	Ks = vec3(1);
	n = 30;

	N = normalize(vec3(normal * vec4 (inNormal, 0)));
	Pp = vec3(modelView * vec4 (inPos, 1));

	color = shade();
	gl_Position =  modelViewProj * vec4 (inPos,1.0);
}
