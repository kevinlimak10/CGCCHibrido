/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Jogos Digitais - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 12/05/2023
 *
 */

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "include/stb_image.h"
#include "include/shader.h"

class Camera
{
public:
	Camera() {}
	~Camera() {}
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


void Camera::inicializar(Shader* shader, int largura, int altura,glm::vec3 cameraFrente, glm::vec3 cameraPos, glm::vec3 cameraCima, float precisao, float inclinacao, float direcao)
{
	prioridadeMouse = true;
	this->shader = shader;
    this->cameraFrente = cameraFrente;
	this->cameraPos = cameraPos;
	this->cameraCima = cameraCima;
	this->precisao = precisao;
	this->inclinacao = inclinacao;
	this->direcao = direcao;

    // Ajustar posição e orientação da câmera
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	shader->setMat4("view", value_ptr(view));

    // Ajustar volume da projeção em perspectiva
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)largura / (float)altura, 0.1f, 100.0f);
	shader->setMat4("projection", glm::value_ptr(projection));
}

void Camera::rotacionar(GLFWwindow* window, double posX, double posY)
{
	if (prioridadeMouse)
	{
		atualX = posX;
		atualY = posY;
		prioridadeMouse = false;
	}

	float deltaX = posX - atualX;
	float deltaY = atualY - posY;

	deltaX *= precisao;
	deltaY *= precisao;
    atualX = posX;
	atualY = posY;
	inclinacao += deltaY;
	direcao += deltaX;

	glm::vec3 frente;
	frente.x = cos(glm::radians(direcao)) * cos(glm::radians(inclinacao));
	frente.y = sin(glm::radians(inclinacao));
	frente.z = sin(glm::radians(direcao)) * cos(glm::radians(inclinacao));
	cameraFrente = glm::normalize(frente);
}

void Camera::movimentar(GLFWwindow* window, int tecla, int acao, float velocidade)
{
	if (tecla == GLFW_KEY_W)
	{
		cameraPos += cameraFrente * velocidade;
	}
	if (tecla == GLFW_KEY_S)
	{
		cameraPos -= cameraFrente * velocidade;
	}
	if (tecla == GLFW_KEY_A)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFrente, cameraCima)) * velocidade;
	}
	if (tecla == GLFW_KEY_D)
	{
		cameraPos += glm::normalize(glm::cross(cameraFrente, cameraCima)) * velocidade;
	}
}

void Camera::sincronizar() {
	//Atualizar a posição e orientação da câmera
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFrente, cameraCima);
	shader->setMat4("view", glm::value_ptr(view));

	//Atualizar o shader com a posição da câmera
	shader->setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);
}


const string PASTA_RECURSOS = "../../Common/3d-models/suzanne/";

struct Face
{
    std::vector<int> verticeIndices;
    std::vector<int> indicesCoordTextura;
    std::vector<int> normalIndices;
};

struct ParsedOBJ
{
    std::vector<float> vertices;
    std::string nomeArquivoMTL;
};

struct Geometria
{
    GLuint VAO;
    int verticesCount;
};

struct CoordTextura
{
    float s, t;
};

struct Vertice
{
    float x, y, z;
};

struct Normal
{
    float x, y, z;
};

struct Material
{
    std::string name;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    string texturePath;
    float shininess;
    int textureId;
};

const std::string ESPACO = " \n\r\t\f\v";

std::string rtrim(const std::string &s)
{
    size_t fim = s.find_last_not_of(ESPACO);
    return (fim == std::string::npos) ? "" : s.substr(0, fim + 1);
}

std::string ltrim(const std::string &s)
{
    size_t inicio = s.find_first_not_of(ESPACO);
    return (inicio == std::string::npos) ? "" : s.substr(inicio);
}

std::string trim(const std::string &s)
{
    return rtrim(ltrim(s));
}

// Protótipo da função de callback de teclado
int setupShader();
int loadTexture(string mtlPath);
ParsedOBJ lerArquivoOBJ(const std::string &mtlPath);
Material lerArquivoMTL(const string &nomeArquivoMTL);
string getTextura(const std::string &nomeArquivoMTL);
Geometria iniciaGeometria(const std::vector<float> &vertices);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);


// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar *vertexShaderSource = "#version 410\n"
                                   "layout (location = 0) in vec3 position;\n"
                                   "layout (location = 1) in vec3 color;\n"
                                   "layout (location = 2) in vec2 tex_coord;\n"
                                   "out vec4 vertexColor;\n"
                                   "out vec2 texCoord;\n"
                                   "uniform mat4 model;\n"
                                   "void main()\n"
                                   "{\n"
                                   //...pode ter mais linhas de código aqui!
                                   "gl_Position = model * vec4(position, 1.0);\n"
                                   "vertexColor = vec4(color, 1.0);\n"
                                   "texCoord = vec2(tex_coord.x, 1 - tex_coord.y);\n"
                                   "}\0";

// Códifo fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar *fragmentShaderSource = "#version 410\n"
                                     "in vec4 vertexColor;\n"
                                     "in vec2 texCoord;\n"
                                     "out vec4 color;\n"
                                     "uniform sampler2D tex_buffer;\n"
                                     "void main()\n"
                                     "{\n"
                                     "color = texture(tex_buffer, texCoord);\n"
                                     "}\n\0";

bool rotacaoX,
    rotacaoY,
    rotacaoZ;

glm::mat4 calculaTransformacoes(glm::mat4 modelo, float angulo)
{
    if (rotacaoX)
    {
        return glm::rotate(modelo, angulo, glm::vec3(1.0f, 0.0f, 0.0f));
    }
    else if (rotacaoY)
    {
        return glm::rotate(modelo, angulo, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else if (rotacaoZ)
    {
        return glm::rotate(modelo, angulo, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    return modelo;
}

// Função MAIN
Camera camera;
int main()
{
    // Inicialização da GLFW
    glfwInit();

    // Muita atenção aqui: alguns ambientes não aceitam essas configurações
    // Você deve adaptar para a versão do OpenGL suportada por sua placa
    // Sugestão: comente essas linhas de código para desobrir a versão e
    // depois atualize (por exemplo: 4.5 com 4 e 5)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Essencial para computadores da Apple
    // #ifdef __APPLE__
    //	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // #endif

    // Criação da janela GLFW
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "3D Cubes", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Fazendo o registro da função de callback para a janela GLFW
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLAD: carrega todos os ponteiros d funções da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    // Obtendo as informações de versão
    const GLubyte *renderer = glGetString(GL_RENDERER); /* get renderer string */
    const GLubyte *version = glGetString(GL_VERSION);   /* version as a string */
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    // Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    ParsedOBJ parsedObj = lerArquivoOBJ(PASTA_RECURSOS + "SuzanneTriTextured.obj");
    Material material = lerArquivoMTL(parsedObj.nomeArquivoMTL);

    Shader shader("vertex-shader.vert", "fragment-shader.frag");

    GLuint textureId = loadTexture(material.texturePath);
    Geometria geometry = iniciaGeometria(parsedObj.vertices);

    GLuint VAO = geometry.VAO;
    int verticesCount = geometry.verticesCount;

    glUseProgram(shader.ID);
    camera.inicializar(&shader, width, height);

    glUniform1i(glGetUniformLocation(shader.ID, "tex_buffer"), 0);

    glm::mat4 model = glm::mat4(1); // matriz identidade;

    glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    shader.setMat4("view", value_ptr(view));

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    shader.setMat4("projection", glm::value_ptr(projection));

    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    shader.setMat4("model", glm::value_ptr(model));

    shader.setVec3("ka", material.ambient.r, material.ambient.g, material.ambient.b);
    shader.setVec3("kd", material.diffuse.r, material.diffuse.g, material.diffuse.b);
    shader.setVec3("ks", material.specular.r, material.specular.g, material.specular.b);
    shader.setFloat("q", material.shininess);

    // Definindo as propriedades da fonte de luz
    shader.setVec3("lightPosition", -5.0f, 15.0f, 5.0f);
    shader.setVec3("lightColor", 1.0f, 1.0f, 0.8f);

    glEnable(GL_DEPTH_TEST);

    // Loop da aplicação - "game loop"
    while (!glfwWindowShouldClose(window))
    {
        // Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
        glfwPollEvents();

        // Limpa o buffer de cor
        glClearColor(0.08f, 0.08f, 0.08f, 1.0f); // cor de fundo
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLineWidth(10);
        glPointSize(20);

        camera.sincronizar();

        float angle = (GLfloat)glfwGetTime();

        model = glm::mat4(1);
        model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
        model = calculaTransformacoes(model, angle);
        shader.setMat4("model", glm::value_ptr(model));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, verticesCount);
        // glDrawArrays(GL_POINTS, 0, verticesCount);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Troca os buffers da tela
        glfwSwapBuffers(window);
    }
    // Pede pra OpenGL desalocar os buffers
    glDeleteVertexArrays(1, &VAO);
    // Finaliza a execução da GLFW, limpando os recursos alocados por ela
    glfwTerminate();
    return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (action == GLFW_PRESS)
    {
        rotacaoX = key == GLFW_KEY_X;
        rotacaoY = key == GLFW_KEY_Y;
        rotacaoZ = key == GLFW_KEY_Z;
    }

    camera.movimentar(window, key, action);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	camera.rotacionar(window, xpos, ypos);
}

// Esta função está basntante hardcoded - objetivo é compilar e "buildar" um programa de
//  shader simples e único neste exemplo de código
//  O código fonte do vertex e fragment shader está nos arrays vertexShaderSource e
//  fragmentShader source no iniçio deste arquivo
//  A função retorna o identificador do programa de shader
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
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
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
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // Linkando os shaders e criando o identificador do programa de shader
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Checando por erros de linkagem
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

ParsedOBJ lerArquivoOBJ(const std::string &nomeArquivo)
{
    std::ifstream file(nomeArquivo);
    std::string linha;
    std::vector<Vertice> vertices;
    std::vector<CoordTextura> coordsTextura;
    std::vector<Face> faces;
    std::string nomeArquivoMTL;
    std::vector<Normal> normais;

    while (std::getline(file, linha))
    {
        std::istringstream iss(linha);
        std::string prefixo;
        iss >> prefixo;

                if (prefixo == "v")
        {
            Vertice vertice;
            iss >> vertice.x >> vertice.y >> vertice.z;
            vertices.push_back(vertice);
        }
        else if (prefixo == "vt")
        {
            CoordTextura texCoord;
            iss >> texCoord.s >> texCoord.t;
            coordsTextura.push_back(texCoord);
        }
        else if (prefixo == "vn")
        {
            Normal normal;
            iss >> normal.x >> normal.y >> normal.z;
            normais.push_back(normal);
        }
        else if (prefixo == "mtllib")
        {
            iss >> nomeArquivoMTL;
        }
        else if (prefixo == "f")
        {
            Face face;
            std::string verticeString;

            while (iss >> verticeString)
            {
                std::istringstream vss(verticeString);
                std::string indiceVerticeString, indiceCoordTexturaString, indiceNormalString;

                std::getline(vss, indiceVerticeString, '/');
                std::getline(vss, indiceCoordTexturaString, '/');
                std::getline(vss, indiceNormalString, '/');

                face.verticeIndices.push_back(std::stoi(indiceVerticeString) - 1);
                if (!indiceCoordTexturaString.empty())
                    face.indicesCoordTextura.push_back(std::stoi(indiceCoordTexturaString) - 1);
                if (!indiceNormalString.empty())
                    face.normalIndices.push_back(std::stoi(indiceNormalString) - 1);
            }

            faces.push_back(face);
        }
    }

    std::vector<float> resultado;

    for (const Face &face : faces)
    {
        for (size_t i = 0; i < face.verticeIndices.size(); ++i)
        {
            const Vertice &vertice = vertices[face.verticeIndices[i]];
            const CoordTextura &textura = coordsTextura[face.indicesCoordTextura[i]];
            const Normal &normal = normais[face.normalIndices[i]];

            resultado.push_back(vertice.x);
            resultado.push_back(vertice.y);
            resultado.push_back(vertice.z);
            resultado.push_back(1);
            resultado.push_back(0);
            resultado.push_back(0);
            resultado.push_back(textura.s);
            resultado.push_back(textura.t);
            resultado.push_back(normal.x);
            resultado.push_back(normal.y);
            resultado.push_back(normal.z);
        }
    }

    ParsedOBJ parsedObj;
    parsedObj.vertices = resultado;
    parsedObj.nomeArquivoMTL = nomeArquivoMTL;

    return parsedObj;
}

string getTextura(const std::string &nomeArquivoMTL)
{
    std::ifstream file(PASTA_RECURSOS + nomeArquivoMTL);
    std::string nomeArquivo;
    std::string linha;

    while (std::getline(file, linha))
    {
        if (linha.find("map_Kd") != std::string::npos)
        {
            size_t pos = linha.find_last_of(" ");
            nomeArquivo = linha.substr(pos + 1);
            break;
        }
    }

    return PASTA_RECURSOS + trim(nomeArquivo);
}

Material lerArquivoMTL(const string &nomeArquivoMTL)
{
    Material material;
    ifstream file(PASTA_RECURSOS + nomeArquivoMTL);

    if (!file.is_open())
    {
        cout << "Failed to open file: " << nomeArquivoMTL << endl;
        return material;
    }

    string linha;
    while (getline(file, linha))
    {
        string palavra;
        istringstream iss(linha);
        iss >> palavra;
        if (palavra == "newmtl")
        {
            iss >> material.name;
        }
        else if (palavra == "Kd" || palavra == "Ke")
        {
            float r, g, b;
            iss >> r >> g >> b;
            material.diffuse = glm::vec3(r, g, b);
        }
        else if (palavra == "map_Kd")
        {
            string fileName;
            iss >> fileName;
            material.texturePath = PASTA_RECURSOS + trim(fileName);
        }
        else if (palavra == "Ka")
        {
            float r, g, b;
            iss >> r >> g >> b;
            material.ambient = glm::vec3(r, g, b);
        }
        else if (palavra == "Ks")
        {
            float r, g, b;
            iss >> r >> g >> b;
            material.specular = glm::vec3(r, g, b);
        }
        else if (palavra == "Ns")
        {
            float shininess;
            iss >> shininess;
            material.shininess = shininess;
        }
    }

    file.close();
    return material;
}

Geometria iniciaGeometria(const std::vector<float> &vertices)
{
    GLuint VBO, VAO;

    // Geração do identificador do VBO
    glGenBuffers(1, &VBO);

    // Faz a conexão (vincula) do buffer como um buffer de array
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Envia os dados do array de floats para o buffer da OpenGl
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Geração do identificador do VAO (Vertex Array Object)
    glGenVertexArrays(1, &VAO);

    // Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
    // e os ponteiros para os atributos
    glBindVertexArray(VAO);

    // Atributo posição (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    // Atributo cor (r, g, b)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Atributo texture (s, t)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // Atributo normal (nx, ny, nz)
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid *)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);

    // Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice
    // atualmente vinculado - para que depois possamos desvincular com segurança
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
    glBindVertexArray(0);

    // Dividido por 11 porque cada vértice tem 11 floats (3 coordenadas, 3 cores, 2 texturas, 3 normais)
    int verticesCount = vertices.size() / 11;

    return {
        VAO,
        verticesCount,
    };
}

int loadTexture(string path)
{
    GLuint texID;

    // Gera o identificador da textura na memória
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    // Ajusta os parâmetros de wrapping e filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Carregamento da imagem
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        if (nrChannels == 3) // jpg, bmp
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else // png
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;
}