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

class Curve
{
public:
	Curve() {}
	inline void atualizarPontosControle(vector<glm::vec3> pontosControle) { this->pontosControle = pontosControle; }
	void atualizarShader(Shader* shader);
	void gerarCurva(int pontosPorSegmento);
	void desenharCurva(glm::vec4 color);
	int getPontosNBCurva() { return pontosCurva.size(); }
	glm::vec3 getPontoNaCurva(int i) { return pontosCurva[i]; }

protected:
	vector<glm::vec3> pontosControle;
	vector<glm::vec3> pontosCurva;
	glm::mat4 M; // Matriz de base
	GLuint VAO;
	Shader* shader;
};