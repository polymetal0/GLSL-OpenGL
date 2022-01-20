#include "BOX.h"
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
int objId =-1;
std::vector<int> assimpModelId;

glm::mat4 view = glm::mat4(1.0);
glm::mat4 proj = glm::mat4(1.0);
glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -40.0f, 0.0f));

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
	if (!IGlib::init("../shaders_P2/shader.focaldir.vert", "../shaders_P2/shader.focaldir.frag"))
		return -1;
	//Se ajusta la cámara
	//Si no se da valor se cojen valores por defecto
	view[3].z = -16.0f;
	IGlib::setViewMat(view);

	/*float f = 1.0f / tan(glm::pi<float>() / 6.0f);
	float far = 5000.0f;
	float near = 0.1f;

	proj[0].x = f;
	proj[1].y = f;
	proj[2].z = (far + near) / (near - far);
	proj[2].w = -1.0f;
	proj[3].z = (2.0f * far * near) / (near - far);
	proj[3].w = 0.0f;
	IGlib::setProjMat(proj);
	IGlib::setViewMat(view);
	*/
	std::vector<float> cvpV;
	for (size_t i = 0; i < cubeNVertex*3; i++)
	{
		cvpV.push_back(cubeVertexPos[i] * 20);
	}
	float* cvp = cvpV.data();
	//Creamos el objeto que vamos a visualizar
	objId = IGlib::createObj(cubeNTriangleIndex, cubeNVertex, cubeTriangleIndex, 
			cvp, cubeVertexColor, cubeVertexNormal, cubeVertexTexCoord, cubeVertexTangent);

	//MODELO ASSIMP
	//std::string modelPath = "../modelos/tallon.fbx";
	//loadModel(modelPath);
	std::string modelPath = "../modelos/SamusDread.fbx";
	loadModel(modelPath);
	for (size_t m = 0; m < assimpModelId.size(); m++)
	{
		IGlib::addColorTex(assimpModelId[m], "../img/Textures/samus_albedo.png");
		IGlib::addSpecularTex(assimpModelId[m], "../img/Textures/samus_PRM.png");
		IGlib::addEmissiveTex(assimpModelId[m], "../img/Textures/samus_E.png");
		IGlib::addNormalTex(assimpModelId[m], "../img/Textures/samus_nrm.png");	}

	IGlib::addColorTex(objId, "../img/color2.png");
	IGlib::addSpecularTex(objId, "../img/specMap.png");
	IGlib::addEmissiveTex(objId, "../img/emissive.png");
	IGlib::addNormalTex(objId, "../img/normal.png");
	
	IGlib::setModelMat(objId, modelMat);

	//CBs
	IGlib::setIdleCB(idleFunc);
	IGlib::setResizeCB(resizeFunc);
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

void idleFunc()
{
	glm::mat4 modelMat(1.0f);
	static float angle = 0.0f;
	angle = (angle > 3.141592f * 2.0f) ? 0 : angle + 0.01f;
	
	//modelMat = glm::rotate(modelMat, angle, glm::vec3(1.0f, 1.0f, 0.0f));

	//IGlib::setModelMat(objId, modelMat);

	for (size_t i = 0; i < assimpModelId.size(); i++)
	{
		glm::mat4 assimpModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -12.0f, 1.5f));//para centrar este modelo en particular
		IGlib::setModelMat(assimpModelId[i], assimpModel);
	}
}

glm::vec3 cop = glm::vec3(0.0f, 0.0f, 16.0f); //view[3])
glm::vec3 lookAt = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

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
	if (state==0)
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

	glm::mat4 rotMat = rotX * rotY;

	view = glm::translate(glm::mat4(1.0f), -cop) * rotMat;
	

	/*//FPS

	float xoffset = x - lastX;
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

	IGlib::setViewMat(view);
}

void loadModel(std::string path) {
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
		unsigned int modelNTriangleIndex = mesh->mNumFaces;
		unsigned int modelNVertex = mesh->mNumVertices;
		std::cout << mesh->mName.data << std::endl;

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

		assimpModelId.push_back(IGlib::createObj(modelNTriangleIndex, modelNVertex, modelTriangleIndex, modelVertexPos, modelVertexColor, modelVertexNormal, modelVertexTexCoord, modelVertexTangent));


	}
}


