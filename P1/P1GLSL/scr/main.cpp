#include "BOX.h"
#include "PYR.h"
#include <IGL/IGlib.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

//Idenficadores de los objetos de la escena
int objId = -1;
int objId2 = -1;
std::vector<int> assimpModelId;

//ViewMatrix
glm::mat4 view = glm::mat4(1.0f);

//ProjMatrix
glm::mat4 proj = glm::mat4(0.0f);

//ModelMatrix 1

//Declaración de CB
void resizeFunc(int width, int height);
void idleFunc();
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunc(int button, int state, int x, int y);
void mouseMotionFunc(int x, int y);
void loadModel(std::string path);

int main(int argc, char** argv)
{
	std::locale::global(std::locale("spanish"));// acentos ;)
	if (!IGlib::init("../shaders_P1/shader.v5.vert", "../shaders_P1/shader.v5.frag"))
		return -1;
   
	//Se ajusta la cámara
	//Si no se da valor se cojen valores por defecto
	//IGlib::setProjMat(const glm::mat4 &projMat);
	//IGlib::setViewMat(const glm::mat4 &viewMat);

	//Creamos el objeto que vamos a visualizar
	objId = IGlib::createObj(cubeNTriangleIndex, cubeNVertex, cubeTriangleIndex, 
			cubeVertexPos, cubeVertexColor, cubeVertexNormal, cubeVertexTexCoord, cubeVertexTangent);
		
	glm::mat4 modelMat = glm::mat4(1.0f);
	IGlib::setModelMat(objId, modelMat);

	//SEGUNDO CUBO
	objId2 = IGlib::createObj(cubeNTriangleIndex, cubeNVertex, cubeTriangleIndex, cubeVertexPos, cubeVertexColor, cubeVertexNormal, cubeVertexTexCoord, cubeVertexTangent);

	//MODELO ASSIMP
	std::string modelPath = "../modelos/SamusDread.fbx";
	loadModel(modelPath);


	//Incluir texturas aquí.
	IGlib::addColorTex(objId, "../img/cow.bmp");

	view[3].z = -16.0f;     //tambien view[3][2]

	IGlib::setViewMat(view);



	//CBs
	IGlib::setResizeCB(resizeFunc);
	IGlib::setIdleCB(idleFunc);
	IGlib::setKeyboardCB(keyboardFunc);
	IGlib::setMouseCB(mouseFunc);
	IGlib::setMouseMoveCB(mouseMotionFunc);

	
	//Mainloop
	IGlib::mainLoop();
	IGlib::destroy();
	return 0;
}


void resizeFunc(int width, int height)
{
	float a = float(width) / float(height);

	float n = 1.0f;
	float f = 5000.0f;

	float x = 1.0f / tan(glm::pi<float>() / 6.0f);

	proj[0].x = x;  //x * 1.0 / a
	proj[1].y = x * a;  //x 
	proj[2].z = (f + n) / (n - f);
	proj[2].w = -1.0f;
	proj[3].z = 2.0f * n * f / (n - f);

	IGlib::setProjMat(proj);
}

float angle = 0.0f;
void idleFunc()
{
	angle = (angle < 2.0f * glm::pi<float>()) ? angle + 0.01f : 0.0f;

	glm::vec3 rotAxis = glm::vec3(1.0f, 1.0f, 0.0f);

	glm::mat4 model = glm::rotate(glm::mat4(1.0f), angle, rotAxis);
	IGlib::setModelMat(objId, model);

	//SEGUNDO CUBO

	glm::vec3 orbit = glm::vec3(0.0f, 0.0f, 6.0f);
	glm::mat4 model2 = glm::translate(model, orbit);							//ORBITAR 
	model2 = glm::rotate(model2, angle, -rotAxis);								//DESHACER ROTACIÓN DEL PRIMER CUBO
	model2 = glm::rotate(model2, 2 * angle, glm::vec3(0.0f, 1.0f, 0.0f));		//ROTAR SOBRE EJE Y

	IGlib::setModelMat(objId2, model2);
	for (size_t i = 0; i < assimpModelId.size(); i++)
	{
		glm::mat4 assimpModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -12.0f, 1.5f));   //para centrar este modelo en particular
		IGlib::setModelMat(assimpModelId[i], assimpModel);
	}
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
	/*POSICIÓN CÁMARA*/
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

	/*ROTACIÓN CÁMARA*/
	case 'q':
		rotation = 15.0f;
		break;
	case 'e':
		rotation = -15.0f;
		break;
	default:
		break;
	}

	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians<float>(rotation), up);
	lookAt = rot * glm::vec4(lookAt, 1.0f);

	view = glm::lookAt(cop, cop + lookAt, up);

	IGlib::setViewMat(view);
}

bool firstClick = true;

void mouseFunc(int button, int state, int x, int y)
{
	if (state == 0) {
		std::cout << "Se ha pulsado el botón ";
	}
		
	else {
		std::cout << "Se ha soltado el botón ";
		firstClick = true;
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
	
	//

	/*//FPS

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

	IGlib::setViewMat(view);
	
}

void loadModel(std::string path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_PreTransformVertices | aiProcess_OptimizeMeshes);

	for (size_t m = 0; m < scene->mNumMeshes; m++)
	{
		std::vector<unsigned int> modelTriIndexVec;
		std::vector<float> modelVertexPosVec;

		std::vector<float> modelVertexColorVec;
		std::vector<float> modelVertexNormalVec;
		std::vector<float> modelVertexTexCoordVec;
		std::vector<float> modelVertexTangentVec;

		aiMesh* mesh = scene->mMeshes[m];
		unsigned int modelNTriangleIndex = mesh->mNumFaces;
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
				modelVertexPosVec.push_back(mesh->mVertices[i].x * 0.008);
				modelVertexPosVec.push_back(mesh->mVertices[i].y * 0.008);
				modelVertexPosVec.push_back(mesh->mVertices[i].z * 0.008);
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
		
		std::cout << mesh->mName.data << std::endl;

		assimpModelId.push_back(IGlib::createObj(modelNTriangleIndex, modelNVertex, modelTriangleIndex, modelVertexPos, modelVertexColor, modelVertexNormal, modelVertexTexCoord, modelVertexTangent));
		IGlib::addColorTex(assimpModelId[m], "../img/Textures/samus_albedo.png");
	}
}
