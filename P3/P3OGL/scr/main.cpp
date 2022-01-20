#include "BOX.h"
#include "auxiliar.h"


#include <gl/glew.h>
#define SOLVE_FGLUT_WARNING
#include <gl/freeglut.h> 

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

//////////////////////////////////////////////////////////////
// Datos que se almacenan en la memoria de la CPU
//////////////////////////////////////////////////////////////

//Matrices
glm::mat4	proj = glm::mat4(1.0f);
glm::mat4	view = glm::mat4(1.0f);
std::vector<glm::mat4>	model;

glm::mat4	model2 = glm::mat4(1.0f);

glm::vec3 lightPos = glm::vec3(0.0, 6.0, 6.0);

int w, h;


//////////////////////////////////////////////////////////////
// Variables que nos dan acceso a Objetos OpenGL
//////////////////////////////////////////////////////////////
//std::vector<unsigned int> vshader;
//std::vector<unsigned int> fshader;

unsigned int vshader;
unsigned int fshader;

std::vector<unsigned int> program;
unsigned int p1;
unsigned int p2;

//Variables Uniform
std::vector<int> uModelViewMat;
std::vector<int> uModelViewProjMat;
std::vector<int> uNormalMat;
//std::vector<int> uViewMat;
std::vector<float> uLightPos;

//Texturas
unsigned int colorTexId;
unsigned int emiTexId;
unsigned int normalTexId;
unsigned int specTexId;

unsigned int modelColorTexId;
unsigned int modelSpecTexId;
unsigned int modelEmiTexId;
unsigned int modelNormalTexId;


//Texturas Uniform
std::vector<int> uColorTex;
std::vector<int> uEmiTex;
std::vector<int> uSpecTex;
std::vector<int> uNormalTex;


//Atributos
std::vector<int> inPos;
std::vector<int> inColor;
std::vector<int> inNormal;
std::vector<int> inTexCoord;

//unsigned int vao;
//VBOs que forman parte del objeto
unsigned int posVBO;
unsigned int colorVBO;
unsigned int normalVBO;
unsigned int texCoordVBO;
unsigned int triangleIndexVBO;

std::vector<unsigned int> vao;
std::vector<unsigned int> modelVaos;
/*VBOs que forman parte del objeto
std::vector<unsigned int> posVBO;
std::vector<unsigned int> colorVBO;
std::vector<unsigned int> normalVBO;
std::vector<unsigned int> texCoordVBO;
std::vector<unsigned int> triangleIndexVBO;*/

std::vector<unsigned int> modelNTriangleIndex;// = mesh->mNumFaces;

//////////////////////////////////////////////////////////////
// Funciones auxiliares
//////////////////////////////////////////////////////////////
//!!Por implementar

//Declaración de CB
void renderFunc();
void resizeFunc(int width, int height);
void idleFunc();
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunc(int button, int state, int x, int y);
void mouseMotionFunc(int x, int y);
void loadModel(std::string path);

//Funciones de inicialización y destrucción
void initContext(int argc, char** argv);
void initOGL();
//void initShader(const std::vector<char*> vname, const std::vector<char*> fname);
void initShader(const char* vname, const char* fname);
void initObj();
void destroy();


//Carga el shader indicado, devuele el ID del shader
//!Por implementar
GLuint loadShader(const char* fileName, GLenum type);

//Crea una textura, la configura, la sube a OpenGL, 
//y devuelve el identificador de la textura 
//!!Por implementar
unsigned int loadTex(const char* fileName);


int main(int argc, char** argv)
{
	std::locale::global(std::locale("spanish"));// acentos ;)

	initContext(argc, argv);
	initOGL();

	//std::vector<char*> vname;
	//std::vector<char*> fname;

	//vname.push_back("../shaders_P3/shader.v1.vert");
	//vname.push_back("../shaders_P3/shader.v0.vert");

	//fname.push_back("../shaders_P3/shader.v1.frag");
	//fname.push_back("../shaders_P3/shader.v0.frag");

	//initShader(vname, fname);
	//initObj();

	

	program.push_back(glCreateProgram());
	//initShader("../shaders_P3/shader.v1.vert", "../shaders_P3/shader.v1.frag");
	initShader("../shaders_P3/shader.focaldir.vert", "../shaders_P3/shader.focaldir.frag");

	initObj();

	program.push_back(glCreateProgram());

	initShader("../shaders_P3/shader.focaldir.vert", "../shaders_P3/shader.focaldir.frag");
	initObj();

	program.push_back(glCreateProgram());
	initShader("../shaders_P3/shader.intensidad.vert", "../shaders_P3/shader.intensidad.frag");
	std::string modelPath = "../modelos/SamusDread.fbx";
	loadModel(modelPath);

	glutMainLoop();


	destroy();

	return 0;
}

//////////////////////////////////////////
// Funciones auxiliares 
void initContext(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Prácticas OGL");


	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		exit(-1);
	}
	const GLubyte* oglVersion = glGetString(GL_VERSION);
	std::cout << "This system supports OpenGL Version: " << oglVersion << std::endl;

	glutReshapeFunc(resizeFunc);
	glutDisplayFunc(renderFunc);
	glutIdleFunc(idleFunc);
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(mouseMotionFunc);
}

void initOGL()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);

	proj = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 50.0f);
	view = glm::mat4(1.0f);
	view[3].z = -16;
}

void destroy()
{
	for (size_t i = 0; i < program.size(); i++)
	{
		glDetachShader(program[i], vshader);
		glDetachShader(program[i], fshader);
		glDeleteShader(vshader);
		glDeleteShader(fshader);
		glDeleteProgram(program[i]);
		glDeleteBuffers(1, &posVBO);
		glDeleteBuffers(1, &colorVBO);
		glDeleteBuffers(1, &normalVBO);
		glDeleteBuffers(1, &texCoordVBO);
		glDeleteBuffers(1, &triangleIndexVBO);
		glDeleteVertexArrays(1, &vao[i]);
	}
	for (size_t mv = 0; mv < modelVaos.size(); mv++)
	{
		glDeleteVertexArrays(1, &modelVaos[mv]);
	}

}

//void initShader(const std::vector<char*> vname, const std::vector<char*> fname)
void initShader(const char* vname, const char* fname)
{
	//for (size_t i = 0; i < 2; i++)
	//{
		//vshader.push_back(loadShader(vname[i], GL_VERTEX_SHADER));
		//fshader.push_back(loadShader(fname[i], GL_FRAGMENT_SHADER));

	vshader = loadShader(vname, GL_VERTEX_SHADER);
	fshader = loadShader(fname, GL_FRAGMENT_SHADER);

	//program.push_back(glCreateProgram());

	glAttachShader(program[program.size() - 1], vshader);
	glAttachShader(program[program.size() - 1], fshader);
	glLinkProgram(program[program.size() - 1]);

	std::cout << program[program.size() - 1] << std::endl;// << p2 << std::endl << p1 << std::endl << std::endl;

	int linked;
	glGetProgramiv(program[program.size() - 1], GL_LINK_STATUS, &linked);
	if (!linked)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetProgramiv(program[program.size() - 1], GL_INFO_LOG_LENGTH, &logLen);
		char* logString = new char[logLen];
		glGetProgramInfoLog(program[program.size() - 1], logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete[] logString;
		glDeleteProgram(program[program.size() - 1]);
		program[program.size() - 1] = 0;
		exit(-1);
	}

	//uViewMat.push_back(glGetUniformLocation(program[program.size() - 1], "view"));
	uNormalMat.push_back(glGetUniformLocation(program[program.size() - 1], "normal"));
	uModelViewMat.push_back(glGetUniformLocation(program[program.size() - 1], "modelView"));
	uModelViewProjMat.push_back(glGetUniformLocation(program[program.size() - 1], "modelViewProj"));

	uColorTex.push_back(glGetUniformLocation(program[program.size() - 1], "colorTex"));
	uEmiTex.push_back(glGetUniformLocation(program[program.size() - 1], "emiTex"));
	uSpecTex.push_back(glGetUniformLocation(program[program.size() - 1], "specularTex"));
	uNormalTex.push_back(glGetUniformLocation(program[program.size() - 1], "normalTex"));

	uLightPos.push_back(glGetUniformLocation(program[program.size() - 1], "lightpos"));

	inPos.push_back(glGetAttribLocation(program[program.size() - 1], "inPos"));
	inColor.push_back(glGetAttribLocation(program[program.size() - 1], "inColor"));
	inNormal.push_back(glGetAttribLocation(program[program.size() - 1], "inNormal"));
	inTexCoord.push_back(glGetAttribLocation(program[program.size() - 1], "inTexCoord"));

	glUseProgram(program[program.size() - 1]);

	//Texturas
	if (uColorTex[program.size() - 1] != -1)
	{
		glUniform1i(uColorTex[program.size() - 1], 0);
	}
	if (uEmiTex[program.size() - 1] != -1)
	{
		glUniform1i(uEmiTex[program.size() - 1], 1);
	}
	if (uSpecTex[program.size() - 1] != -1)
	{
		glUniform1i(uSpecTex[program.size() - 1], 2);
	}
	if (uNormalTex[program.size() - 1] != -1)
	{
		glUniform1i(uNormalTex[program.size() - 1], 3);
	}

	//}

}


void initObj()
{
	//posVBO = 0;// .resize(program.size());
	//colorVBO = 0;//.resize(program.size());
	//normalVBO = 0;//.resize(program.size());
	//texCoordVBO = 0;//.resize(program.size());
	//triangleIndexVBO = 0;//.resize(program.size());
	vao.resize(program.size());

	std::vector<float> cvpV;
	for (size_t i = 0; i < cubeNVertex * 3; i++)
	{
		cvpV.push_back(cubeVertexPos[i] * 20);
	}
	float* cvp = cvpV.data();
	//for (size_t i = 0; i < program.size(); i++)
	//{
	glUseProgram(program[program.size() - 1]);

	glGenBuffers(1, &posVBO);
	glGenBuffers(1, &colorVBO);
	glGenBuffers(1, &normalVBO);
	glGenBuffers(1, &texCoordVBO);

	glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 2,
		cubeVertexTexCoord, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao[program.size() - 1]);
	glBindVertexArray(vao[program.size() - 1]);

	glBindBuffer(GL_ARRAY_BUFFER, posVBO);
	glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3,
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, cubeNVertex * sizeof(float) * 3,
		cvp);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	if (inPos[program.size() - 1] != -1)
		glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3,
		cubeVertexColor, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	if (inColor[program.size() - 1] != -1)
		glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3,
		cubeVertexNormal, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	if (inNormal[program.size() - 1] != -1)
		glEnableVertexAttribArray(2);


	glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
	if (inTexCoord[program.size() - 1] != -1)
		glEnableVertexAttribArray(3);

	glGenBuffers(1, &triangleIndexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		cubeNTriangleIndex * sizeof(unsigned int) * 3, cubeTriangleIndex,
		GL_STATIC_DRAW);

	colorTexId = loadTex("../img/color2.png");
	emiTexId = loadTex("../img/emissive.png");
	specTexId = loadTex("../img/specMap.png");
	normalTexId = loadTex("../img/normal.png");

	model.push_back(glm::mat4(1.0f));
	//}
}

GLuint loadShader(const char* fileName, GLenum type)
{
	unsigned int fileLen;
	char* source = loadStringFromFile(fileName, fileLen);

	//////////////////////////////////////////////
	//Creación y compilación del Shader
	GLuint shader;
	shader = glCreateShader(type);
	glShaderSource(shader, 1,
		(const GLchar**)&source, (const GLint*)&fileLen);
	glCompileShader(shader);
	delete[] source;

	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
		char* logString = new char[logLen];
		glGetShaderInfoLog(shader, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete[] logString;
		glDeleteShader(shader);
		exit(-1);
	}

	return shader;
}

unsigned int loadTex(const char* fileName)
{
	unsigned char* map;
	unsigned int w, h;
	map = loadTexture(fileName, w, h);
	if (!map)
	{
		std::cout << "Error cargando el fichero: "
			<< fileName << std::endl;
		exit(-1);
	}

	unsigned int texId;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, (GLvoid*)map);

	delete[] map;

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	return texId;
}

void renderFunc()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, w, h);

	//lightPos = glm::vec3(glm::vec4(lightPos, 1.0) * view);

	int objs = 0;
	for (size_t i = 0; i < vao.size(); i++)
	{
		//glViewport(0, 0, w/2, h);

		glUseProgram(program[i]);

		glm::mat4 modelView = view * model[i];
		glm::mat4 modelViewProj = proj * modelView;
		glm::mat4 normal = glm::transpose(glm::inverse(modelView));

		if (uModelViewMat[i] != -1) {
			glUniformMatrix4fv(uModelViewMat[i], 1, GL_FALSE,
				&(modelView[0][0]));
		}

		if (uModelViewProjMat[i] != -1) {
			glUniformMatrix4fv(uModelViewProjMat[i], 1, GL_FALSE,
				&(modelViewProj[0][0]));
		}

		if (uNormalMat[i] != -1) {
			glUniformMatrix4fv(uNormalMat[i], 1, GL_FALSE,
				&(normal[0][0]));
		}
		if (uLightPos[i] != -1)
		{
			glUniform3f(uLightPos[i], lightPos[0], lightPos[1], lightPos[2]);
			//glUniform3fv(uLightPos[i], 1, &lightPos[0]);
		}


		//Texturas
		if (uColorTex[i] != -1)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, colorTexId);
		}
		if (uEmiTex[i] != -1)
		{
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, emiTexId);
		}
		if (uSpecTex[i] != -1)
		{
			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, specTexId);
		}
		if (uNormalTex[i] != -1)
		{
			glActiveTexture(GL_TEXTURE0 + 3);
			glBindTexture(GL_TEXTURE_2D, normalTexId);
		}

		glBindVertexArray(vao[i]);
		glDrawElements(GL_TRIANGLES, cubeNTriangleIndex * 3,
			GL_UNSIGNED_INT, (void*)0);

		objs += 1;
	}

	if (program.size() > objs)
	{
		glUseProgram(program[objs]);

		glm::mat4 modelView = view * model[objs];
		glm::mat4 modelViewProj = proj * modelView;
		glm::mat4 normal = glm::transpose(glm::inverse(modelView));

		if (uModelViewMat[objs] != -1) {
			glUniformMatrix4fv(uModelViewMat[objs], 1, GL_FALSE,
				&(modelView[0][0]));
		}

		if (uModelViewProjMat[objs] != -1) {
			glUniformMatrix4fv(uModelViewProjMat[objs], 1, GL_FALSE,
				&(modelViewProj[0][0]));
		}

		if (uNormalMat[objs] != -1) {
			glUniformMatrix4fv(uNormalMat[objs], 1, GL_FALSE,
				&(normal[0][0]));
		}
		if (uLightPos[objs] != -1)
		{
			glUniform3f(uLightPos[objs], lightPos[0], lightPos[1], lightPos[2]);
			//glUniform3fv(uLightPos[objs], 1, 
				//&lightPos[0]);
		}

		for (size_t i = 0; i < modelVaos.size(); i++)
		{
			//Texturas
			if (uColorTex[objs] != -1)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, modelColorTexId);
			}
			if (uEmiTex[objs] != -1)
			{
				glActiveTexture(GL_TEXTURE0 + 1);
				glBindTexture(GL_TEXTURE_2D, modelEmiTexId);
			}
			if (uSpecTex[objs] != -1)
			{
				glActiveTexture(GL_TEXTURE0 + 2);
				glBindTexture(GL_TEXTURE_2D, modelSpecTexId);
			}
			if (uNormalTex[objs] != -1)
			{
				glActiveTexture(GL_TEXTURE0 + 3);
				glBindTexture(GL_TEXTURE_2D, modelNormalTexId);
			}

			glBindVertexArray(modelVaos[i]);
			glDrawElements(GL_TRIANGLES, modelNTriangleIndex[i] * 3,
				GL_UNSIGNED_INT, (void*)0);
		}

	}

	/*//SEGUNDO CUBO
	modelView = view * model2;
	modelViewProj = proj * modelView;
	normal = glm::transpose(glm::inverse(modelView));
	//
	if (uModelViewMat != -1)
		glUniformMatrix4fv(uModelViewMat, 1, GL_FALSE,
			&(modelView[0][0]));
	if (uModelViewProjMat != -1)
		glUniformMatrix4fv(uModelViewProjMat, 1, GL_FALSE,
			&(modelViewProj[0][0]));
	if (uNormalMat != -1)
		glUniformMatrix4fv(uNormalMat, 1, GL_FALSE,
			&(normal[0][0]));

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, cubeNTriangleIndex * 3,
		GL_UNSIGNED_INT, (void*)0);*/

		/**glViewport(w / 2, 0, w / 2, h);
		//Subir uniforms
		//Render calls
		glm::mat4 v = view;
		v[3].x += 0.1f;
		modelView = v * model;
		modelViewProj = proj * modelView;
		normal = glm::transpose(glm::inverse(modelView));

		if (uModelViewMat != -1)
			glUniformMatrix4fv(uModelViewMat, 1, GL_FALSE,
				&(modelView[0][0]));
		if (uModelViewProjMat != -1)
			glUniformMatrix4fv(uModelViewProjMat, 1, GL_FALSE,
				&(modelViewProj[0][0]));
		if (uNormalMat != -1)
			glUniformMatrix4fv(uNormalMat, 1, GL_FALSE,
				&(normal[0][0]));

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, cubeNTriangleIndex * 3,
			GL_UNSIGNED_INT, (void*)0);/**/

	glutSwapBuffers();
}

void resizeFunc(int width, int height)
{
	//glViewport(0, 0, width, height);
	w = width;
	h = height;
	float a = float(w) / float(h);

	float n = 1.0f;
	float f = 5000.0f;

	float x = 1.0f / tan(glm::pi<float>() / 6.0f);

	proj[0].x = x;  //x * 1.0 / a
	proj[1].y = x * a;  //x 
	proj[2].z = (f + n) / (n - f);
	proj[2].w = -1.0f;
	proj[3].z = 2.0f * n * f / (n - f);

	glutPostRedisplay();

}

void idleFunc()
{

	model[0] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -40.0f, 0.0f));
	static float angle = 0.0f;
	angle = (angle > 3.141592f * 2.0f) ? 0 : angle + 0.01f;
	model[0] = glm::rotate(model[0], angle, glm::vec3(1.0f, 1.0f, 0.0f));

	//MATRIZ MODELO PARA QUE EL SEGUNDO CUBO ORBITE AL PRIMERO Y GIRE SOBRE SU EJE Y
	glm::vec3 rotAxis = glm::vec3(1.0f, 1.0f, 0.0f);
	glm::vec3 orbit = glm::vec3(0.0f, 0.0f, 60.0f);

	model[1] = glm::translate(model[0], orbit);							//ORBITAR (INDEPENDIENTEMENTE DE CAMBIOS SOBRE PRIMER CUBO)
	model[1] = glm::rotate(model[1], angle, -rotAxis);								//DESHACER ROTACIÓN DEL PRIMER CUBO
	model[1] = glm::rotate(model[1], 2 * angle, glm::vec3(0.0f, 1.0f, 0.0f));		//ROTAR SOBRE EJE Y
	
	model[2] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -12.0f, 1.5f));//para centrar este modelo en particular
	
	glutPostRedisplay();
}

glm::vec3 lookAt = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cop = glm::vec3(0.0f, 0.0f, 16.0f);

void keyboardFunc(unsigned char key, int x, int y)
{
	float cameraSpeed = 1.0;

	float rotation = 0.0f;
	std::cout << "Se ha pulsado la tecla " << key << std::endl << std::endl;

	switch (key)
	{
	case 'w':
		cop += cameraSpeed * lookAt;
		break;
	case 'a':
		cop -= glm::normalize(glm::cross(lookAt, up)) * cameraSpeed;
		break;
	case 's':
		cop -= cameraSpeed * lookAt;
		break;
	case 'd':
		cop += glm::normalize(glm::cross(lookAt, up)) * cameraSpeed;
		break;

		/*ROTACIÓN CÁMARA CON TECLADO*/
	case 'q':
		rotation = 15.0f;
		break;
	case 'e':
		rotation = -15.0f;
		break;
	case '+':
		lightPos += glm::vec3(0.0, 1.0, 0.0);
		break;
	case '-':
		lightPos -= glm::vec3(0.0, 1.0, 0.0);
		break;
		//model = model2;
		//program = glCreateProgram();
		//initShader("../shaders_P3/shader.v0.vert", "../shaders_P3/shader.v0.frag", 2);
		//initObj();
	default:
		break;
	}

	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians<float>(rotation), up);
	lookAt = rot * glm::vec4(lookAt, 1.0f);

	view = glm::lookAt(cop, cop + lookAt, up);
	glUniform3f(uLightPos[program.size()-1], lightPos[0], lightPos[1], lightPos[2]);
	glutPostRedisplay();
}


bool firstClick = true;

void mouseFunc(int button, int state, int x, int y)
{
	if (state == 0)
		std::cout << "Se ha pulsado el botón ";
	else {
		firstClick = true;
		std::cout << "Se ha soltado el botón ";
	}

	if (button == 0) std::cout << "de la izquierda del ratón " << std::endl;
	if (button == 1) std::cout << "central del ratón " << std::endl;
	if (button == 2) std::cout << "de la derecha del ratón " << std::endl;

	std::cout << "en la posición " << x << " " << y << std::endl << std::endl;
}

float lastX = 0.0f, lastY = 0.0f;
float yaw = -90.0f;
float pitch = 0.0f;

void mouseMotionFunc(int x, int y)
{
	//CÁMARA ORBITAL

	
	float camX = sin(x) * 16.0f;
	float camZ = cos(x) * 16.0f;

	view = glm::lookAt(cop, glm::vec3(0.0f, 0.0f, 0.0f) + lookAt, glm::vec3(0.0f, 1.0f, 0.0f));
	float xoffset = x - lastX;
	float yoffset = lastY - y;

	if (firstClick)
	{
		xoffset = 0.0f;
		yoffset = 0.0f;
		firstClick = false;
	}

	lastX = x;
	lastY = y;

	float sensitivity = 0.01f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), pitch, glm::cross(lookAt, up));
	glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), yaw, up);

	//lookAt = glm::normalize(rotationX * rotationY * glm::vec4(lookAt, 0));
	//up = glm::normalize(rotationX * rotationY * glm::vec4(up, 0));
	//right = glm::normalize(glm::cross(lookAt, up));
	//view = glm::inverse(glm::mat4(glm::vec4(right, 0), glm::vec4(up, 0), glm::vec4(-lookAt, 0), glm::vec4(cop, 0)));

	glm::mat4 rotMat = rotX * rotY;

	view = glm::translate(glm::mat4(1.0f), -cop) * rotMat;
	//cop.z = -view[3].z;*/
	//

	//FPS

	/*float xoffset = x - lastX;
	float yoffset = lastY - y; // reversed since y-coordinates go from bottom to top

	if (firstClick)
	{
		xoffset = 0.0f;
		yoffset = 0.0f;
		firstClick = false;
	}

	lastX = x;
	lastY = y;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	lookAt = glm::normalize(front);

	view = glm::lookAt(cop, cop + lookAt, up);
	//*/

	glutPostRedisplay;
}

std::vector<int> assimpModelId;

void loadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_PreTransformVertices | aiProcess_OptimizeMeshes | aiProcess_CalcTangentSpace);
	float scale = 0.008;

	glUseProgram(program[program.size() - 1]);

	for (size_t m = 0; m < scene->mNumMeshes; m++)
	{
		std::vector<unsigned int> modelTriIndexVec;
		std::vector<float> modelVertexPosVec;

		std::vector<float> modelVertexColorVec;
		std::vector<float> modelVertexNormalVec;
		std::vector<float> modelVertexTexCoordVec;
		std::vector<float> modelVertexTangentVec;

		aiMesh* mesh = scene->mMeshes[m];
		//unsigned int 
		modelNTriangleIndex.push_back(mesh->mNumFaces);
		unsigned int modelNVertex = mesh->mNumVertices;

		for (size_t i = 0; i < mesh->mNumFaces; i++)
		{
			for (size_t j = 0; j < mesh->mFaces->mNumIndices; j++)
			{
				modelTriIndexVec.push_back(mesh->mFaces[i].mIndices[j]);
			}

		}

		for (size_t i = 0; i < mesh->mNumVertices; i++)
		{
			if (mesh->HasPositions())
			{
				modelVertexPosVec.push_back(mesh->mVertices[i].x * scale);
				modelVertexPosVec.push_back(mesh->mVertices[i].y * scale);
				modelVertexPosVec.push_back(mesh->mVertices[i].z * scale);
			}
			if (mesh->HasNormals())
			{
				modelVertexNormalVec.push_back(mesh->mNormals[i].x);
				modelVertexNormalVec.push_back(mesh->mNormals[i].y);
				modelVertexNormalVec.push_back(mesh->mNormals[i].z);
			}
			if (mesh->HasTextureCoords(0))
			{
				modelVertexTexCoordVec.push_back(mesh->mTextureCoords[0][i].x);
				modelVertexTexCoordVec.push_back(mesh->mTextureCoords[0][i].y);
			}
			if (mesh->HasVertexColors(i))
			{
				modelVertexColorVec.push_back(mesh->mColors[i]->r);
				modelVertexColorVec.push_back(mesh->mColors[i]->g);
				modelVertexColorVec.push_back(mesh->mColors[i]->b);
			}
			if (mesh->HasTangentsAndBitangents())
			{
				modelVertexTangentVec.push_back(mesh->mTangents[i].x);
				modelVertexTangentVec.push_back(mesh->mTangents[i].y);
				modelVertexTangentVec.push_back(mesh->mTangents[i].z);

			}

		}
		unsigned int* modelTriangleIndex = modelTriIndexVec.data();
		float* modelVertexPos = modelVertexPosVec.data();
		float* modelVertexNormal = modelVertexNormalVec.data();
		float* modelVertexColor = modelVertexColorVec.data();
		float* modelVertexTexCoord = modelVertexTexCoordVec.data();
		float* modelVertexTangent = modelVertexTangentVec.data();

		//posVBO = 0;// .resize(program.size());
		//colorVBO = 0;//.resize(program.size());
		//normalVBO = 0;//.resize(program.size());
		//texCoordVBO = 0;//.resize(program.size());
		//triangleIndexVBO = 0;//.resize(program.size());
		modelVaos.push_back(0);// resize(program.size());

		glGenBuffers(1, &posVBO);
		glGenBuffers(1, &colorVBO);
		glGenBuffers(1, &normalVBO);
		glGenBuffers(1, &texCoordVBO);

		glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, modelNVertex * sizeof(float) * 2,
			modelVertexTexCoord, GL_STATIC_DRAW);

		glGenVertexArrays(1, &modelVaos[modelVaos.size() - 1]);
		glBindVertexArray(modelVaos[modelVaos.size() - 1]);

		glBindBuffer(GL_ARRAY_BUFFER, posVBO);
		glBufferData(GL_ARRAY_BUFFER, modelNVertex * sizeof(float) * 3,
			NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, modelNVertex * sizeof(float) * 3,
			modelVertexPos);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		if (inPos[program.size() - 1] != -1)
			glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glBufferData(GL_ARRAY_BUFFER, modelNVertex * 3,//sizeof(float) * 3,
			modelVertexColor, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		if (inColor[program.size() - 1] != -1)
			glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glBufferData(GL_ARRAY_BUFFER, modelNVertex * sizeof(float) * 3,
			modelVertexNormal, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		if (inNormal[program.size() - 1] != -1)
			glEnableVertexAttribArray(2);


		glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
		if (inTexCoord[program.size() - 1] != -1)
			glEnableVertexAttribArray(3);

		glGenBuffers(1, &triangleIndexVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexVBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			modelNTriangleIndex[modelNTriangleIndex.size()-1] * sizeof(unsigned int) * 3,
			modelTriangleIndex, GL_STATIC_DRAW);

	}

	modelColorTexId = loadTex("../img/Textures/samus_albedo.png");
	modelSpecTexId = loadTex("../img/Textures/samus_PRM.png");
	modelEmiTexId = loadTex("../img/Textures/samus_E.png");
	modelNormalTexId = loadTex("../img/Textures/samus_nrm.png");

	model.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -12.0f, 1.5f)));
}








