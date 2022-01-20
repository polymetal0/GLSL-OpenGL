#version 330 core

out vec4 outColor;
in vec3 color;


//Propiedades de las funtes de luz
//uniforms
vec3 Ia = vec3(0.1);
vec3 Il = vec3(100.0);
vec3 lpos = vec3(-2.0, 0.0, 0.0);//Coord cámara

vec3 Ia2 = vec3(0.1);
vec3 Il2 = vec3(100.0);
vec3 lpos2 = vec3(2.0, 0.0, 0.0);

//Propiedades del objeto
vec3 Ka;
vec3 Kd;
vec3 Ks;
float n;
vec3 N;
vec3 pos;

vec3 Ka2;
vec3 Kd2;
vec3 Ks2;
float n2;
vec3 N2;
vec3 pos2;

vec3 shade()
{
	vec3 c = vec3(0);
	vec3 c2 = vec3(0);

	//Ambiental
	c += Ia*Ka;
	c2 += Ia2*Ka2;

	//Difusa
	vec3 L = normalize(lpos - pos);
	vec3 L2 = normalize(lpos2 - pos2);
	c += Il * Kd * max(dot(N, L), 0);// * (pow(1/(length(pos - lpos)+1), 2));
	c2 += Il2 * Kd2 * max(dot(N2, L2), 0);// * (pow(1/(length(pos2 - lpos2)+1), 2));

	//Especular
	vec3 V = normalize(-pos);
	vec3 V2 = normalize(-pos2);
	//vec3 R = normalize(reflect(-L,N));
	//c+= Il * Ks * pow (max(dot(V, R),0), n);
	vec3 H = normalize(V+L);
	vec3 H2 = normalize(V2+L2);
	c += Il * Ks * pow (max(dot(H, N),0), n);// * (pow((1/(length(pos - lpos)+1)), 2));
	c2 += Il2 * Ks2 * pow (max(dot(H2, N2),0), n2);// * (pow(1/(length(pos2 - lpos2)+1), 2));

	return c + c2;// + c2 
}

in vec3 vNormal;
in vec3 vPos;
in vec3 vColor;

void main()
{
	Ka = vColor;
	Kd = Ka;// * (pow((1/length(vPos - lpos)+1), 2));
	Ks = vec3(1,0,0);
	n = 200;

	Ka2 = vColor;
	Kd2 = Ka;// * (pow((1/length(vPos - lpos2)+1), 2));
	Ks2 = vec3(0,1,0);
	n2 = 200;

	N = normalize(vNormal);
	N2 = normalize(vNormal);
	pos = vPos;
	pos2 = vPos;

	outColor = vec4(shade(), 1.0);   
}
