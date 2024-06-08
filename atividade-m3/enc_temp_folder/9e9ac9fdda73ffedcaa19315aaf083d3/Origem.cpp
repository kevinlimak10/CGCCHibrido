/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Jogos Digitais - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 12/05/2023
 *
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Protótipos das funções
int setupShader();
int setupGeometry();
glm::mat4 applyScale(glm::mat4 currentMatrix, float scaleFactorPercentage);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar* vertexShaderSource = "#version 450\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"uniform mat4 model;\n"
"out vec4 finalColor;\n"
"void main()\n"
"{\n"
//...pode ter mais linhas de código aqui!
"gl_Position = model * vec4(position, 1.0);\n"
"finalColor = vec4(color, 1.0);\n"
"}\0";

//Códifo fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar* fragmentShaderSource = "#version 450\n"
"in vec4 finalColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = finalColor;\n"
"}\n\0";

glm::vec3 translation(0.0f, 0.0f, 0.0f);
std::vector<glm::vec3> cubePositions;

void addCubeInstance(float x, float y, float z) {
	cubePositions.push_back(glm::vec3(x, y, z));
}

bool rotateX = false, rotateY = false, rotateZ = false, scaleUp = false, scaleDown = false;
bool moveX = false, moveY = false, moveZ = false;
// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	//Muita atenção aqui: alguns ambientes não aceitam essas configurações
	//Você deve adaptar para a versão do OpenGL suportada por sua placa
	//Sugestão: comente essas linhas de código para desobrir a versão e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Essencial para computadores da Apple
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola 3D -- Kevin Lima!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();

	// Gerando um buffer simples, com a geometria de um triângulo
	GLuint VAO = setupGeometry();


	glUseProgram(shaderID);

	glm::mat4 model = glm::mat4(1); //matriz identidade;
	GLint modelLoc = glGetUniformLocation(shaderID, "model");
	//
	model = glm::rotate(model, /*(GLfloat)glfwGetTime()*/glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

	glEnable(GL_DEPTH_TEST);

	addCubeInstance(0.0f, 0.0f, 0.0f);

	model = glm::mat4(1);
	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		float angle = (GLfloat)glfwGetTime();
		for (auto& position : cubePositions) {
			
			if (rotateX) {
				model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
			}
			if (rotateY) {
				model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			if (rotateZ) {
				model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
			}

			translation.x, translation.y, translation.z = 0;
			if (moveX) {
				translation.x = 0.1f;
			}
			if (moveY) {
				translation.y = 0.1f;
			}
			if (moveZ) {
				translation.z = 0.1f;
			}
			
			model = glm::translate(model, glm::vec3(translation.x + position.x, translation.y + position.y, translation.z + position.z));

			if (scaleUp) {
				model = applyScale(model, 1.5);
			}
			else if (scaleDown) {
				model = applyScale(model, 0.5);
			}

			glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
			// Chamada de desenho - drawcall
			// Poligono Preenchido - GL_TRIANGLES

			// Chamada de desenho - drawcall
			// Poligono Preenchido - GL_TRIANGLES
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

			// Chamada de desenho - drawcall
			// CONTORNO - GL_LINE_LOOP
			glDrawElements(GL_POINTS, 36, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			moveX,moveY,moveZ = false;
		}

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Função para aplicar a escala a uma matriz de transformação atual
glm::mat4 applyScale(glm::mat4 currentMatrix, float scaleFactorPercentage) {

	// Cria uma matriz de escala
	glm::mat4 scaleMatrix = glm::scale(currentMatrix, glm::vec3(scaleFactorPercentage, scaleFactorPercentage, scaleFactorPercentage));

	// Aplica a transformação de escala à matriz atual
	return scaleMatrix;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		rotateX = true;
		rotateY = false;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = true;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = false;
		rotateZ = true;
	}
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		moveZ = true;
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		moveZ = false;
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		moveX = true;
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		moveX = false;
	}
	if (key == GLFW_KEY_I && action == GLFW_PRESS) {
		moveY = true;
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		moveY = false;
	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS) {
		scaleUp = !scaleUp;
	}
	if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS) {
		scaleDown = !scaleDown;
	}

	if (key == GLFW_KEY_N && action == GLFW_PRESS)
		addCubeInstance(0.0f, 0.0f, 0.0f);

}

//Esta função está basntante hardcoded - objetivo é compilar e "buildar" um programa de
// shader simples e único neste exemplo de código
// O código fonte do vertex e fragment shader está nos arrays vertexShaderSource e
// fragmentShader source no iniçio deste arquivo
// A função retorna o identificador do programa de shader
int setupShader()
{
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Checando erros de compilação (exibição via log no terminal)
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Checando erros de compilação (exibição via log no terminal)
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Linkando os shaders e criando o identificador do programa de shader
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Checando por erros de linkagem
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

// Esta função está bastante harcoded - objetivo é criar os buffers que armazenam a 
// geometria de um triângulo
// Apenas atributo coordenada nos vértices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO
int setupGeometry()
{
	// Coordenadas x, y, z e cores r, g, b para os vértices do cubo
	GLfloat vertices[] = {
		// Face frontal - triângulo 1
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		 // Face frontal - triângulo 2
		  0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
		 -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
		 -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,

		 // Face traseira - triângulo 1
		 -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		  0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		  // Face traseira - triângulo 2
		   0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		   0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		  -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,

		  // Face superior - triângulo 1
		  -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
		   0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		   // Face superior - triângulo 2
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		   -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,

		   // Face inferior - triângulo 1
		   -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
			// Face inferior - triângulo 2
			 0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,

			// Face direita - triângulo 1
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
			 // Face direita - triângulo 2
			  0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
			  0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
			  0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,

			  // Face esquerda - triângulo 1
			  -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
			  -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
			  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
			  // Face esquerda - triângulo 2
			  -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
			  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
			  -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
	};

	GLuint indices[] = {
		// Frente
		0, 1, 2,
		2, 3, 0,
		// Traseira
		4, 5, 6,
		6, 7, 4,
		// Superior
		8, 9, 10,
		10, 11, 8,
		// Inferior
		12, 13, 14,
		14, 15, 12,
		// Direita
		16, 17, 18,
		18, 19, 16,
		// Esquerda
		20, 21, 22,
		22, 23, 20,
	};

	GLuint VBO, VAO, EBO;

	// Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Geração do identificador do VBO (Vertex Buffer Object)
	glGenBuffers(1, &VBO);
	// Geração do identificador do EBO (Element Buffer Object)
	glGenBuffers(1, &EBO);

	// Vincula o VAO primeiro
	glBindVertexArray(VAO);

	// Vincula e alimenta o VBO com os vértices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Vincula e alimenta o EBO com os índices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Atributo posição
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Atributo cor
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Desvincula o VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO
	glBindVertexArray(0);

	return VAO;
}