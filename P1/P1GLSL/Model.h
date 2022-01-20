�#pragma once
#include <alegre / glad.h> // todos los archivos de encabezado 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define stb_image_implementation // El autor original no escribi�
#include <stb_image.h>
#include <assimp / import.hpp> // Archivo de encabezado de la biblioteca de assiMP
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
// leer textura del archivo
unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);
// Clase modelo
class Model
{
public:
	/ *Datos del modelo * /
		// Tienda todas las texturas que est�n cargadas hasta ahora, optimizan para asegurarse de que la textura no se cargar� varias veces.
		vector<Texture> textures_loaded;
	vector<Mesh> meshes;
	string directory;
	bool gammaCorrection;

	/ *Funci�n * /
		// Chino conforme, necesita una ruta de archivo de un modelo 3D
		Model(string const& path, bool gamma = false) : gammaCorrection(gamma)
	{
		loadModel(path);
	}

	// Dibuja un modelo para dibujar todas las redes.
	void Draw(Shader shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

private:
	/ *Funci�n * /
		// Cargue la extensi�n de Assimp desde el archivo y almacene la cuadr�cula generada en el vector de la cuadr�cula.
		void loadModel(string const& path)
	{
		// leer archivos a trav�s de assimp
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		// cheque por errores
		Si(!Escena || escena-> mflags & ai_scene_flags_incumplete || escena-> mrootNode) // si no 0
		{
			Cout << "  :: assimp ::" << Import.getRRRORRING() << Endl;
			return;
		}
		/ / Recuperar la ruta del directorio de la ruta del archivo
			directory = path.substr(0, path.find_last_of('/'));

		// tratar el nodo ra�z de assintes en modo recursivo
		processNode(scene->mRootNode, scene);
	}

	// manejar el nodo de una manera recursiva. Procesando cada cuadr�cula individual en el nodo y repitiendo este proceso en su nodo secundario (si corresponde).
	void processNode(aiNode* node, const aiScene* scene)
	{
		// procesar cada cuadr�cula ubicada en el nodo actual
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// El objeto de nodo solo contiene el objeto real indexado en el escenario.
			// La escena contiene todos los datos, el nodo es solo para guardar las organizaciones (como la relaci�n entre nodos).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		// Despu�s de manejar todas las cuadr�culas (si las hay), manejamos recursivamente a cada nodo secundario
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// ser rellenado
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;

		// atravesar los v�rtices de cada cuadr�cula
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			// Declaramos un vector de marcador de posici�n, porque Assim se usa su propia clase de vectores, no se convierte directamente a la clase VEC3 de GLM, por lo que primero pasamos los datos a este marcador de posici�n GLM :: Vec3.
			glm::vec3 vector;
			// Localizaci�n
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			// normal
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			// coordenadas de textura
			Si(malla-> mtexturecoords[0]) // �La cuadr�cula incluye coordenadas de textura?
			{
				glm::vec2 vec;
				// El v�rtice puede contener hasta 8 coordenadas de textura diferentes. Por lo tanto, asumimos que no usaremos el v�rtice para tener un modelo de m�ltiples coordenadas de textura, por lo que siempre usamos la primera colecci�n (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			// u vector
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// v vector
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
			vertices.push_back(vertex);
		}
		// Ahora, ahora est� atravesado por cada cuadr�cula (una es una cuadr�cula triangular) y recupera el �ndice de v�rtice correspondiente.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// todos los �ndices de los fideos y almacenarlos en el vector �ndice
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// material de procesamiento
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		/ / Asumimos que se acuerda el nombre de la muestra en el sombreado.Cada textura de reflexi�n difusa debe denominarse 'texture_diffusen', donde n es un n�mero de serie de 1 a max_sampler_number.
			/ / Lo mismo se aplica a otras texturas, como se resume :
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN

		 // 1. Mapa de reflexi�n
		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. Mapa de alto brillo
		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. Mapa franc�s
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. alta textura
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		/ / Devuelva el objeto de cuadr�cula creado a partir de los datos de cuadr�cula extra�dos.
			return Mesh(vertices, indices, textures);
	}

	// Marque todas las texturas de material del tipo dado. Si la textura no est� cargada, la textura est� cargada.
	// La informaci�n requerida se devuelve como la estructura de la textura.
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// Compruebe si se carga la textura, en caso afirmativa, contin�e con la siguiente iteraci�n: omita la nueva textura de carga
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true;
					Break; // Se ha cargado la textura con la misma ruta del archivo y se contin�a la siguiente (optimizaci�n).
				}
			}
			if (!skip)
			{// Si no ha cargado textura, por favor carguelo.
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				Textures_loaded.push_back(textura); // lo almacena como una textura cargada en todo el modelo para asegurarse de que no cargamos la textura de repetici�n.
			}
		}
		return textures;
	}
};
// leer la funci�n de textura del archivo
unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		STD::COUT << "Textura No se puede cargar desde esta ruta:" << ruta << Std::endl;
		stbi_image_free(data);
	}
	return textureID;
}
