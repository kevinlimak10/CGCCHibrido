// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "include/shader.h"

class CameraRecord
{
public:
	CameraRecord() {}
	~CameraRecord() {}
	void inicializar(Shader* shader, int largura, int altura, glm::vec3 cameraFrente = glm::vec3(0.0, 0.0, -1.0), glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0), glm::vec3 cameraCima = glm::vec3(0.0, 1.0, 0.0), float precisao = 0.03, float inclinacao = 0.0, float direcao = -90.0);
	void rotacionar(GLFWwindow* window, double posX, double posY);
	void movimentar(GLFWwindow* window, int tecla, int acao, float velocidade = 0.08);
	void sincronizar();

protected:
	Shader* shader;
	bool prioridadeMouse;
	float atualX, atualY, inclinacao, direcao;
	float precisao;
	glm::vec3 cameraFrente, cameraPos, cameraCima;
};