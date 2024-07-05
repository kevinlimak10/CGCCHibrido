#include <vector>
// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "include/shader.h"

class Mesh
{
public:
	Mesh() {}
	~Mesh() {}
	void inicializar(GLuint VAO, int nVertices, Shader* shader, GLuint textureID, glm::vec3 posicao = glm::vec3(0.0, 0.0, 0.0), glm::vec3 escala = glm::vec3(0.5, 0.5, 0.5), float angulo = 0.0, glm::vec3 eixo = glm::vec3(0.0, 0.0, 1.0));
	void sincronizar();
	void desenhar(Material material);
	void sincronizarPosicao(glm::vec3 posicao);
	void atualizarRotacionarY(bool deveRotacionarY);

protected:
	GLuint VAO; // Vertex Array Object
	int nVertices;

	// Informações sobre as transformações a serem aplicadas no objeto
	glm::vec3 posicao;
	glm::vec3 escala;
	float angulo;
	glm::vec3 eixo;

	// Referência (endereço) do shader
	Shader* shader;

	GLuint textureID;

	bool deveRotacionarY = false;
};