/* Hello Triangle - c�digo adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gr�fico - Jogos Digitais - Unisinos
 * Vers�o inicial: 7/4/2017
 * �ltima atualiza��o em 12/05/2023
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


// Prot�tipo da fun��o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Prot�tipos das fun��es
int setupShader();
int setupGeometry();
glm::mat4 applyScale(glm::mat4 currentMatrix, float scaleFactorPercentage);

// Dimens�es da janela (pode ser alterado em tempo de execu��o)
const GLuint WIDTH = 1000, HEIGHT = 1000;

// C�digo fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar* vertexShaderSource = "#version 450\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"uniform mat4 model;\n"
"out vec4 finalColor;\n"
"void main()\n"
"{\n"
//...pode ter mais linhas de c�digo aqui!
"gl_Position = model * vec4(position, 1.0);\n"
"finalColor = vec4(color, 1.0);\n"
"}\0";

//C�difo fonte do Fragment Shader (em GLSL): ainda hardcoded
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
// Fun��o MAIN
int main()
{
	// Inicializa��o da GLFW
	glfwInit();

	//Muita aten��o aqui: alguns ambientes n�o aceitam essas configura��es
	//Voc� deve adaptar para a vers�o do OpenGL suportada por sua placa
	//Sugest�o: comente essas linhas de c�digo para desobrir a vers�o e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Essencial para computadores da Apple
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

	// Cria��o da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola 3D -- Kevin Lima!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da fun��o de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d fun��es da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informa��es de vers�o
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();

	// Gerando um buffer simples, com a geometria de um tri�ngulo
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
	// Loop da aplica��o - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as fun��es de callback correspondentes
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
	// Finaliza a execu��o da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Fun��o para aplicar a escala a uma matriz de transforma��o atual
glm::mat4 applyScale(glm::mat4 currentMatrix, float scaleFactorPercentage) {

	// Cria uma matriz de escala
	glm::mat4 scaleMatrix = glm::scale(currentMatrix, glm::vec3(scaleFactorPercentage, scaleFactorPercentage, scaleFactorPercentage));

	// Aplica a transforma��o de escala � matriz atual
	return scaleMatrix;
}

// Fun��o de callback de teclado - s� pode ter uma inst�ncia (deve ser est�tica se
// estiver dentro de uma classe) - � chamada sempre que uma tecla for pressionada
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

//Esta fun��o est� basntante hardcoded - objetivo � compilar e "buildar" um programa de
// shader simples e �nico neste exemplo de c�digo
// O c�digo fonte do vertex e fragment shader est� nos arrays vertexShaderSource e
// fragmentShader source no ini�io deste arquivo
// A fun��o retorna o identificador do programa de shader
int setupShader()
{
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Checando erros de compila��o (exibi��o via log no terminal)
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
	// Checando erros de compila��o (exibi��o via log no terminal)
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

// Esta fun��o est� bastante harcoded - objetivo � criar os buffers que armazenam a 
// geometria de um tri�ngulo
// Apenas atributo coordenada nos v�rtices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A fun��o retorna o identificador do VAO
int setupGeometry()
{
	// Coordenadas x, y, z e cores r, g, b para os v�rtices do cubo
	GLfloat vertices[] = {
		// Face frontal - tri�ngulo 1
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		 // Face frontal - tri�ngulo 2
		  0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
		 -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
		 -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,

		 // Face traseira - tri�ngulo 1
		 -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		  0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		  // Face traseira - tri�ngulo 2
		   0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		   0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		  -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,

		  // Face superior - tri�ngulo 1
		  -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
		   0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		   // Face superior - tri�ngulo 2
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		   -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,

		   // Face inferior - tri�ngulo 1
		   -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
			// Face inferior - tri�ngulo 2
			 0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,

			// Face direita - tri�ngulo 1
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
			 // Face direita - tri�ngulo 2
			  0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
			  0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
			  0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,

			  // Face esquerda - tri�ngulo 1
			  -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
			  -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
			  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
			  // Face esquerda - tri�ngulo 2
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

	// Gera��o do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Gera��o do identificador do VBO (Vertex Buffer Object)
	glGenBuffers(1, &VBO);
	// Gera��o do identificador do EBO (Element Buffer Object)
	glGenBuffers(1, &EBO);

	// Vincula o VAO primeiro
	glBindVertexArray(VAO);

	// Vincula e alimenta o VBO com os v�rtices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Vincula e alimenta o EBO com os �ndices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Atributo posi��o
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