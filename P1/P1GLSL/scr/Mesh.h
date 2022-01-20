#pragma once
#include <alegre / glad.h> // todos los archivos de encabezado
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;
// vértice
struct Vertex {
	// Localización
	glm::vec3 Position;
	// método vector
	glm::vec3 Normal;
	// coordenadas de textura
	glm::vec2 TexCoords;
	// u vector
	glm::vec3 Tangent;
	// v vector
	glm::vec3 Bitangent;
};
// textura
struct Texture {
	unsigned int id;
	string type;
	string path;
};
// clase de malla
class Mesh {
public:
	/*Datos de malla */
		vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO;

	/*Función */
		// Parámetros de la función constructiva: Textura del índice de vértices
		Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		// Ahora tenemos todos los datos requeridos, configure el búfer de vértices y su puntero de propiedad.
		setupMesh();
	}

	// Modelo de cuadrícula de pintura
	void Draw(Shader shader)
	{
		/ / Enlazar la textura apropiada
			unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			Glactivetexture(gl_texture0 + i); // Active la unidad de textura apropiada antes de encuadernar
											  // obtener números de textura (n) en difuse_texturen
			string number;
			string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);
			else if (name == "texture_normal")
				number = std::to_string(normalNr++);
			else if (name == "texture_height")
				number = std::to_string(heightNr++);
			/ / Ahora coloque el muestreador a la unidad de textura correcta
				glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
			// la última textura obligada
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		// pintura cuadrícula
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		/ / Una vez que se completa la configuración, siempre es una buena práctica para recuperar todo.
			glActiveTexture(GL_TEXTURE0);
	}

private:
	/*Datos de representación */
		unsigned int VBO, EBO;

	/*Función */
		// Inicializar todos los objetos de búfer / matrices
		void setupMesh()
	{
		// crear búfer / matriz
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// cargar los datos en el búfer de vértices
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		/ / Un beneficio sobre la estructura es que su diseño de memoria es secuencial para todos sus artículos.
			/ / El resultado es que podemos simplemente pasar el puntero a la estructura, y se convierte perfectamente en la matriz GLM::VEC3 / 2, la matriz se convierte en un número de punto flotante 3 / 2, se convierte en matrices de bytes.
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		/ / Establecer el puntero de la propiedad Vertex
			// Posición del vértice
			glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normal
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// coordenadas de textura de vértices
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// u vector
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// v vector
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		glBindVertexArray(0);
	}
};