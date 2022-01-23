#include "BOX.h"
#include "auxiliar.h"
#include "PLANE.h"
#include "MASKS.h"

#include <gl/glew.h>
#define SOLVE_FGLUT_WARNING
#include <gl/freeglut.h> 

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <cstdlib>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

#define RAND_SEED 31415926
#define SCREEN_SIZE 500,500

//////////////////////////////////////////////////////////////
// Datos que se almacenan en la memoria de la CPU
//////////////////////////////////////////////////////////////

//Matrices
glm::mat4	proj = glm::mat4(1.0f);
glm::mat4	view = glm::mat4(1.0f);
glm::mat4	model = glm::mat4(1.0f);

glm::vec3 lightPos = glm::vec3(0.0, 6.0, 6.0);
glm::vec3 lightInt = glm::vec3(1000.0, 1000.0, 1000.0);
glm::vec4 motionBlur = glm::vec4(0.5, 0.5, 0.5, 0.5);

float focalDistance = -25.0;
float maxDistanceFactor = 0.2;

float* mask = identity;

//float maskFactor = 1.0f / 190.0f;
//////////////////////////////////////////////////////////////
// Variables que nos dan acceso a Objetos OpenGL
//////////////////////////////////////////////////////////////
float angle = 0.0f;

//VAO
unsigned int vao;

std::vector<unsigned int> modelNTriangleIndex;
std::vector<unsigned int> modelVaos;

//VBOs que forman parte del objeto
unsigned int posVBO;
unsigned int colorVBO;
unsigned int normalVBO;
unsigned int texCoordVBO;
unsigned int triangleIndexVBO;

unsigned int colorTexId;
unsigned int emiTexId;
unsigned int specTexId;
unsigned int normalTexId;

//
unsigned int planeVAO;
unsigned int planeVertexVBO;

//Por definir
unsigned int vshader;
unsigned int fshader;
unsigned int program;

//Variables Uniform 
int uModelViewMat;
int uModelViewProjMat;
int uNormalMat;
std::vector<int> uLightPos;
std::vector<int> uLightInt;

//Texturas Uniform
int uColorTex;
int uEmiTex;
int uSpecTex;
int uNormalTex;

//Atributos
int inPos;
int inColor;
int inNormal;
int inTexCoord;


unsigned int postProccesVShader;
unsigned int postProccesFShader;
unsigned int postProccesProgram;
//Uniform
unsigned int uColorTexPP;

unsigned int fbo;
unsigned int colorBuffTexId;
unsigned int depthBuffTexId;

unsigned int uZTexPP;
unsigned int zBuffTexId;

unsigned int uFocalDist;
unsigned int uMaxDistFactor;

unsigned int uConvMask;

//////////////////////////////////////////////////////////////
// Funciones auxiliares
//////////////////////////////////////////////////////////////

//Declaración de CB
void renderFunc();
void resizeFunc(int width, int height);
void idleFunc();
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunc(int button, int state, int x, int y);
void mouseMotionFunc(int x, int y);

void renderCube();
void renderModel();

//Funciones de inicialización y destrucción
void initContext(int argc, char** argv);
void initOGL();
void initShaderFw(const char *vname, const char *fname);
void initObj();

void loadModel(std::string path);

void destroy();


//Carga el shader indicado, devuele el ID del shader
//!Por implementar
GLuint loadShader(const char *fileName, GLenum type);
GLfloat maxAnisotropic;

//Crea una textura, la configura, la sube a OpenGL, 
//y devuelve el identificador de la textura 
//!!Por implementar
unsigned int loadTex(const char *fileName);

//////////////////////////////////////////////////////////////
// Nuevas variables auxiliares
//////////////////////////////////////////////////////////////
void initPlane();
void initShaderPP(const char* vname, const char* fname);
void initFBO();
void resizeFBO(unsigned int w, unsigned int h);

//////////////////////////////////////////////////////////////
// Nuevas funciones auxiliares
//////////////////////////////////////////////////////////////
//!!Por implementar


int main(int argc, char** argv)
{
	std::locale::global(std::locale("spanish"));// acentos ;)

	initContext(argc, argv);
	initOGL();
	initShaderFw("../shaders_P4/fwRendering.v1.vert", "../shaders_P4/fwRendering.v1.frag");
	initObj();
	//std::string modelPath = "../modelos/SamusDread.fbx";
	//loadModel(modelPath);

	initShaderPP("../shaders_P4/postProcessing.v0.vert", "../shaders_P4/postProcessing.v1.frag");
	initPlane();
	initFBO();
	resizeFBO(SCREEN_SIZE);


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
	//glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_SIZE);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Prácticas GLSL");

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		exit(-1);
	}

	const GLubyte *oglVersion = glGetString(GL_VERSION);
	std::cout << "This system supports OpenGL Version: " << oglVersion << std::endl;

	if (glewIsSupported("GL_EXT_texture_filter_anisotropic"))
	{
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropic);
	}

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

	proj = glm::perspective(glm::radians(60.0f), 1.0f, 1.0f, 50.0f);
	view = glm::mat4(1.0f);
	view[3].z = -25.0f;
}


void destroy()
{
	glDetachShader(program, vshader);
	glDetachShader(program, fshader);
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	glDeleteProgram(program);

	if (inPos != -1) glDeleteBuffers(1, &posVBO);
	if (inColor != -1) glDeleteBuffers(1, &colorVBO);
	if (inNormal != -1) glDeleteBuffers(1, &normalVBO);
	if (inTexCoord != -1) glDeleteBuffers(1, &texCoordVBO);
	glDeleteBuffers(1, &triangleIndexVBO);

	glDeleteVertexArrays(1, &vao);

	glDeleteTextures(1, &colorTexId);
	glDeleteTextures(1, &emiTexId);
}

void initShaderFw(const char *vname, const char *fname)
{
	vshader = loadShader(vname, GL_VERTEX_SHADER);
	fshader = loadShader(fname, GL_FRAGMENT_SHADER);

	program = glCreateProgram();
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);

	glBindAttribLocation(program, 0, "inPos");
	glBindAttribLocation(program, 1, "inColor");
	glBindAttribLocation(program, 2, "inNormal");
	glBindAttribLocation(program, 3, "inTexCoord");


	glLinkProgram(program);

	int linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);

		char *logString = new char[logLen];
		glGetProgramInfoLog(program, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete[] logString;

		glDeleteProgram(program);
		program = 0;
		exit(-1);
	}

	uNormalMat = glGetUniformLocation(program, "normal");
	uModelViewMat = glGetUniformLocation(program, "modelView");
	uModelViewProjMat = glGetUniformLocation(program, "modelViewProj");

	uColorTex = glGetUniformLocation(program, "colorTex");
	uEmiTex = glGetUniformLocation(program, "emiTex");
	uSpecTex = glGetUniformLocation(program, "specularTex");
	uNormalTex = glGetUniformLocation(program, "normalTex");

	inPos = glGetAttribLocation(program, "inPos");
	inColor = glGetAttribLocation(program, "inColor");
	inNormal = glGetAttribLocation(program, "inNormal");
	inTexCoord = glGetAttribLocation(program, "inTexCoord");
}

void initObj()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	if (inPos != -1)
	{
		glGenBuffers(1, &posVBO);
		glBindBuffer(GL_ARRAY_BUFFER, posVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex*sizeof(float) * 3,
			cubeVertexPos, GL_STATIC_DRAW);
		glVertexAttribPointer(inPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inPos);
	}

	if (inColor != -1)
	{
		glGenBuffers(1, &colorVBO);
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex*sizeof(float) * 3,
			cubeVertexColor, GL_STATIC_DRAW);
		glVertexAttribPointer(inColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inColor);
	}

	if (inNormal != -1)
	{
		glGenBuffers(1, &normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex*sizeof(float) * 3,
			cubeVertexNormal, GL_STATIC_DRAW);
		glVertexAttribPointer(inNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inNormal);
	}


	if (inTexCoord != -1)
	{
		glGenBuffers(1, &texCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex*sizeof(float) * 2,
			cubeVertexTexCoord, GL_STATIC_DRAW);
		glVertexAttribPointer(inTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inTexCoord);
	}

	glGenBuffers(1, &triangleIndexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		cubeNTriangleIndex*sizeof(unsigned int) * 3, cubeTriangleIndex,
		GL_STATIC_DRAW);

	model = glm::mat4(1.0f);

	colorTexId = loadTex("../img/color2.png");
	emiTexId = loadTex("../img/emissive.png");
	specTexId = loadTex("../img/specMap.png");
	normalTexId = loadTex("../img/normal.png");
}

GLuint loadShader(const char *fileName, GLenum type)
{
	unsigned int fileLen;
	char *source = loadStringFromFile(fileName, fileLen);

	//////////////////////////////////////////////
	//Creación y compilación del Shader
	GLuint shader;
	shader = glCreateShader(type);
	glShaderSource(shader, 1,
		(const GLchar **)&source, (const GLint *)&fileLen);
	glCompileShader(shader);
	delete[] source;

	//Comprobamos que se compilo bien
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

		char *logString = new char[logLen];
		glGetShaderInfoLog(shader, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete[] logString;

		glDeleteShader(shader);
		exit(-1);
	}

	return shader;
}

unsigned int loadTex(const char *fileName)
{
	unsigned char *map;
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
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropic);

	return texId;
}

void renderFunc()
{

	
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/**/
	glUseProgram(program);

	//Texturas
	if (uColorTex != -1)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorTexId);
		glUniform1i(uColorTex, 0);
	}

	if (uEmiTex != -1)
	{
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, emiTexId);
		glUniform1i(uEmiTex, 1);
	}
	if (uSpecTex != -1)
	{
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, specTexId);
		glUniform1i(uSpecTex, 2);
	}
	if (uNormalTex != -1)
	{
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, normalTexId);
		glUniform1i(uNormalTex, 3);
	}
	//model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -12.0f, 1.5f));

	model = glm::mat4(2.0f);
	model[3].w = 1.0f;
	model = glm::rotate(model, angle, glm::vec3(1.0f, 1.0f, 0.0f));

	renderCube();
	//renderModel();

	std::srand(RAND_SEED);
	for (unsigned int i = 0; i < 10; i++)
	{
		float size = float(std::rand() % 3 + 1);

		glm::vec3 axis(glm::vec3(float(std::rand() % 2),
			float(std::rand() % 2), float(std::rand() % 2)));
		if (glm::all(glm::equal(axis, glm::vec3(0.0f))))
			axis = glm::vec3(1.0f);

		float trans = float(std::rand() % 7 + 3) * 1.00f + 0.5f;
		glm::vec3 transVec = axis * trans;
		transVec.x *= (std::rand() % 2) ? 1.0f : -1.0f;
		transVec.y *= (std::rand() % 2) ? 1.0f : -1.0f;
		transVec.z *= (std::rand() % 2) ? 1.0f : -1.0f;

		//model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -12.0f, 1.5f));

		model = glm::rotate(glm::mat4(1.0f), angle*2.0f*size, axis);
		model = glm::translate(model, transVec);
		model = glm::rotate(model, angle*2.0f*size, axis);
		model = glm::scale(model, glm::vec3(1.0f / (size*0.7f)));

		renderCube();
		//renderModel();
	}
	//*/

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(postProccesProgram);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendEquation(GL_FUNC_ADD);

	glEnable(GL_BLEND);
	glBlendFunc(GL_CONSTANT_COLOR, GL_CONSTANT_ALPHA);
	glBlendColor(motionBlur.r,motionBlur.g,motionBlur.b,motionBlur.a);
	glBlendEquation(GL_FUNC_ADD);

	
	if (uColorTexPP != -1)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffTexId);
		glUniform1i(uColorTexPP, 0);
	}

	if (uZTexPP != -1)
	{
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, zBuffTexId);
		glUniform1i(uZTexPP, 1);
	}

	if (uFocalDist != -1)
	{
		glUniform1f(uFocalDist, focalDistance);
	}

	if (uMaxDistFactor != -1)
	{
		glUniform1f(uMaxDistFactor, maxDistanceFactor);
	}
	
	if (uConvMask != -1)
	{
		glUniform1fv(uConvMask, 9, &mask[0]);
	}

	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	

	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);


	glutSwapBuffers();
}

void renderCube()
{
	glm::mat4 modelView = view * model;
	glm::mat4 modelViewProj = proj * view * model;
	glm::mat4 normal = glm::transpose(glm::inverse(modelView));

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
		GL_UNSIGNED_INT, (void*)0);
}

void renderModel()
{
	glm::mat4 modelView = view * model;
	glm::mat4 modelViewProj = proj * view * model;
	glm::mat4 normal = glm::transpose(glm::inverse(modelView));

	if (uModelViewMat != -1)
		glUniformMatrix4fv(uModelViewMat, 1, GL_FALSE,
		&(modelView[0][0]));
	if (uModelViewProjMat != -1)
		glUniformMatrix4fv(uModelViewProjMat, 1, GL_FALSE,
		&(modelViewProj[0][0]));
	if (uNormalMat != -1)
		glUniformMatrix4fv(uNormalMat, 1, GL_FALSE,
		&(normal[0][0]));
	
	for (size_t i = 0; i < modelVaos.size(); i++)
	{
		glBindVertexArray(modelVaos[i]);
		glDrawElements(GL_TRIANGLES, modelNTriangleIndex[i] * 3,
			GL_UNSIGNED_INT, (void*)0);
	}
}



void resizeFunc(int width, int height)
{
	glViewport(0, 0, width, height);
	proj = glm::perspective(glm::radians(60.0f), float(width) /float(height), 1.0f, 5000.0f);

	resizeFBO(width, height);

	glutPostRedisplay();
}

void idleFunc()
{
	angle = (angle > 3.141592f * 2.0f) ? 0 : angle + 0.02f;
	
	glutPostRedisplay();
}

glm::vec3 lookAt = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cop = glm::vec3(0.0f, 0.0f, 25.0f);

void keyboardFunc(unsigned char key, int x, int y)
{
	float cameraSpeed = 1.0;

	float rotation = 0.0f;
	std::cout << "Se ha pulsado la tecla " << key << std::endl << std::endl;

	switch (key)
	{
		//MOVIMIENTO CÁMARA
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
		//ROTACIÓN CÁMARA
	case 'q':
		rotation = 15.0f;
		break;
	case 'e':
		rotation = -15.0f;
		break;
	//MOTION BLUR
	case 'f':
		motionBlur.r += 0.1f;
		break;
	case 'g':
		motionBlur.g += 0.1f;
		break;
	case 'h':
		motionBlur.b += 0.1f;
		break;
	case 'j':
		motionBlur.a += 0.1f;
		break;

	case 'c':
		motionBlur.r -= 0.1f;
		break;
	case 'v':
		motionBlur.g -= 0.1f;
		break;
	case 'b':
		motionBlur.b -= 0.1f;
		break;
	case 'n':
		motionBlur.a -= 0.1f;
		break;
	//DOF
	case 'k':
		focalDistance += 1.0f;
		break;
	case 'm':
		focalDistance -= 1.0f;
		break;

	case 'l':
		maxDistanceFactor += 0.1f;
		break;
	case ',':
		maxDistanceFactor -= 0.1f;
		break;
	//CONVOLUTION
	case 'y':
		mask = identity;
		break;
	case 'u':
		mask = blur;
		break;
	case 'i':
		mask = sharpen;
		break;
	case 'o':
		mask = emboss;
		break;
	case 'p':
		mask = outline;
		break;		

	default:
		break;
	}

	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians<float>(rotation), up);
	lookAt = rot * glm::vec4(lookAt, 1.0f);

	view = glm::lookAt(cop, cop + lookAt, up);

	if (uFocalDist != -1)
	{
		glUniform1f(uFocalDist, focalDistance);
	}

	if (uMaxDistFactor != -1)
	{
		glUniform1f(uMaxDistFactor, maxDistanceFactor);
	}

	std::cout << focalDistance << std::endl;
	std::cout << maxDistanceFactor << std::endl;

	//std::cout << motionBlur[0] << std::endl;
	//std::cout << motionBlur[1] << std::endl;
	//std::cout << motionBlur[2] << std::endl;
	//std::cout << motionBlur[3] << std::endl;

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

	pitch = glm::clamp(pitch, -89.9f, 89.9f);

	glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), pitch, glm::cross(lookAt, up));
	glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), yaw, up);

	glm::mat4 rotMat = rotX * rotY;

	view = glm::translate(glm::mat4(1.0f), -cop) * rotMat;

	//FPS

	/*float xoffset = x - lastX;
	float yoffset = lastY - y;

	if (firstClick)
	{
		xoffset = 0.0f;
		yoffset = 0.0f;
		firstClick = false;
	}

	lastX = x;
	lastY = y;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	pitch = glm::clamp(pitch, -89.9f, 89.9f);

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	lookAt = glm::normalize(front);

	view = glm::lookAt(cop, cop + lookAt, up);
	//*/

	glutPostRedisplay;
}
void initPlane()
{

	glGenBuffers(1, &planeVertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVertexVBO);
	glBufferData(GL_ARRAY_BUFFER, planeNVertex * sizeof(float) * 3,
		planeVertexPos, GL_STATIC_DRAW);

	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
}

void initShaderPP(const char* vname, const char* fname)
{
	postProccesVShader = loadShader(vname, GL_VERTEX_SHADER);
	postProccesFShader = loadShader(fname, GL_FRAGMENT_SHADER);

	postProccesProgram = glCreateProgram();
	glAttachShader(postProccesProgram, postProccesVShader);
	glAttachShader(postProccesProgram, postProccesFShader);
	
	glLinkProgram(postProccesProgram);
	int linked;
	glGetProgramiv(postProccesProgram, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetProgramiv(postProccesProgram, GL_INFO_LOG_LENGTH, &logLen);
		char* logString = new char[logLen];
		glGetProgramInfoLog(postProccesProgram, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete logString;
		glDeleteProgram(postProccesProgram);
		postProccesProgram = 0;
		exit(-1);
	}

	uColorTexPP = glGetUniformLocation(postProccesProgram, "colorTex");
	uZTexPP = glGetUniformLocation(postProccesProgram, "zTex");

	uFocalDist = glGetUniformLocation(postProccesProgram, "focalDist");
	uMaxDistFactor = glGetUniformLocation(postProccesProgram, "distFactor");

	uConvMask = glGetUniformLocation(postProccesProgram, "msk");

	if (0 != glGetAttribLocation(postProccesProgram, "inPos"))
		exit(-1);

}


void initFBO()
{
	glGenFramebuffers(1, &fbo);
	glGenTextures(1, &colorBuffTexId);
	glGenTextures(1, &depthBuffTexId);
	glGenTextures(1, &zBuffTexId);
}

void resizeFBO(unsigned int w, unsigned int h)
{
	glBindTexture(GL_TEXTURE_2D, colorBuffTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0,
		GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);


	glBindTexture(GL_TEXTURE_2D, zBuffTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, w, h, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, depthBuffTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0,
		GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, colorBuffTexId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, zBuffTexId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
		depthBuffTexId, 0);

	const GLenum buffs[2] = { GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(2, buffs);
	
	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
		std::cerr << "Error configurando el FBO" << std::endl;
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

std::vector<int> assimpModelId;


void loadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_PreTransformVertices | aiProcess_OptimizeMeshes | aiProcess_CalcTangentSpace);
	float scale = 0.008;

	for (size_t m = 0; m < scene->mNumMeshes; m++)
	{
		std::vector<unsigned int> modelTriIndexVec;
		std::vector<float> modelVertexPosVec;

		std::vector<float> modelVertexColorVec;
		std::vector<float> modelVertexNormalVec;
		std::vector<float> modelVertexTexCoordVec;
		std::vector<float> modelVertexTangentVec;

		aiMesh* mesh = scene->mMeshes[m];
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

		modelVaos.push_back(0);

		glGenVertexArrays(1, &modelVaos[modelVaos.size() - 1]);
		glBindVertexArray(modelVaos[modelVaos.size() - 1]);

		if (inPos != -1)
		{
			glGenBuffers(1, &posVBO);
			glBindBuffer(GL_ARRAY_BUFFER, posVBO);
			glBufferData(GL_ARRAY_BUFFER, modelNVertex * sizeof(float) * 3,
				modelVertexPos, GL_STATIC_DRAW);
			glVertexAttribPointer(inPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(inPos);
		}

		if (inColor != -1)
		{

			glGenBuffers(1, &colorVBO);
			glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
			glBufferData(GL_ARRAY_BUFFER, modelNVertex * 3, //sizeof(float) *
				modelVertexColor, GL_STATIC_DRAW);
			glVertexAttribPointer(inColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(inColor);
		}

		if (inNormal != -1)
		{

			glGenBuffers(1, &normalVBO);
			glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
			glBufferData(GL_ARRAY_BUFFER, modelNVertex * sizeof(float) * 3,
				modelVertexNormal, GL_STATIC_DRAW);
			glVertexAttribPointer(inNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(inNormal);

		}


		if (inTexCoord != -1)
		{
			glGenBuffers(1, &texCoordVBO);
			glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
			glBufferData(GL_ARRAY_BUFFER, modelNVertex * sizeof(float) * 2,
				modelVertexTexCoord, GL_STATIC_DRAW); 
			glVertexAttribPointer(inTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(inTexCoord);
		}

		glGenBuffers(1, &triangleIndexVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexVBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			modelNTriangleIndex[modelNTriangleIndex.size() - 1] * sizeof(unsigned int) * 3,
			modelTriangleIndex, GL_STATIC_DRAW);

		model = glm::mat4(1.0f);

		colorTexId = loadTex("../img/color2.png");
		emiTexId = loadTex("../img/emissive.png");

	}

	colorTexId = loadTex("../img/Textures/samus_albedo.png");
	specTexId = loadTex("../img/Textures/samus_PRM.png");
	emiTexId = loadTex("../img/Textures/samus_E.png");
	normalTexId = loadTex("../img/Textures/samus_nrm.png");

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -12.0f, 1.5f));
}