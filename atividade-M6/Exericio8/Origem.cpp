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
#include "include/camera.h"
#include "include/bezier.h"
#include "include/mesh.h"

void Camera::inicializar(Shader *shader, int largura, int altura, glm::vec3 cameraFrente, glm::vec3 cameraPos, glm::vec3 cameraCima, float precisao, float inclinacao, float direcao)
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

void Camera::rotacionar(GLFWwindow *window, double posX, double posY)
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

void Camera::movimentar(GLFWwindow *window, int tecla, int acao, float velocidade)
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

void Camera::sincronizar()
{
  // Atualizar a posição e orientação da câmera
  glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFrente, cameraCima);
  shader->setMat4("view", glm::value_ptr(view));

  // Atualizar o shader com a posição da câmera
  shader->setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);
}

void Curva::atualizarShader(Shader *shader)
{
  this->shader = shader;
  shader->Use();
}

void Curva::desenharCurva(glm::vec4 color)
{
  shader->setVec4("finalColor", color.r, color.g, color.b, color.a);

  glBindVertexArray(VAO);
  // Chamada de desenho - drawcall
  // CONTORNO e PONTOS - GL_LINE_LOOP e GL_POINTS
  glDrawArrays(GL_LINE_STRIP, 0, pontosCurva.size());
  glBindVertexArray(0);
}

Bezier::Bezier()
{
  M = glm::mat4(
      -1, 3, -3, 1,
      3, -6, 3, 0,
      -3, 3, 0, 0,
      1, 0, 0, 0);
}

void Bezier::gerarCurva(int pointsPerSegment)
{
  float step = 1.0 / (float)pointsPerSegment;

  float t = 0;

  int nControlPoints = pontosControle.size();

  for (int i = 0; i < nControlPoints - 3; i += 3)
  {

    for (float t = 0.0; t <= 1.0; t += step)
    {
      glm::vec3 p;

      glm::vec4 T(t * t * t, t * t, t, 1);

      glm::vec3 P0 = pontosControle[i];
      glm::vec3 P1 = pontosControle[i + 1];
      glm::vec3 P2 = pontosControle[i + 2];
      glm::vec3 P3 = pontosControle[i + 3];

      glm::mat4x3 G(P0, P1, P2, P3);

      p = G * M * T; //---------

      pontosCurva.push_back(p);
    }
  }

  // Gera o VAO
  GLuint VBO;

  // Gera o identificador do VBO
  glGenBuffers(1, &VBO);

  // Faz a conexão (vincula) do buffer como um buffer de array
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // Envia os dados do array de floats para o buffer da OpenGl
  glBufferData(GL_ARRAY_BUFFER, pontosCurva.size() * sizeof(GLfloat) * 3, pontosCurva.data(), GL_STATIC_DRAW);

  // Gera o identificador do VAO (Vertex Array Object)
  glGenVertexArrays(1, &VAO);

  // Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
  // e os ponteiros para os atributos
  glBindVertexArray(VAO);

  // Atributo posição (x, y, z)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
  glEnableVertexAttribArray(0);

  // Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice
  // atualmente vinculado - para que depois possamos desvincular com segurança
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
  glBindVertexArray(0);
}

void Malha::inicializar(GLuint VAO, int nVertices, Shader *shader, GLuint textureID, glm::vec3 posicao, glm::vec3 escala, float angulo, glm::vec3 eixo)
{
  this->VAO = VAO;
  this->nVertices = nVertices;
  this->shader = shader;
  this->posicao = posicao;
  this->escala = escala;
  this->angulo = angulo;
  this->eixo = eixo;
  this->textureID = textureID;
  this->deveRotacionarY = false;
}

void Malha::sincronizarPosicao(glm::vec3 posicao)
{
  this->posicao = posicao;
}

void Malha::atualizarRotacionarY(bool deveRotacionarY)
{
  this->deveRotacionarY = deveRotacionarY;
}

void Malha::sincronizar()
{
  glm::mat4 model = glm::mat4(1);
  model = glm::translate(model, posicao);

  if (deveRotacionarY)
  {
    this->angulo += 0.001;
    model = glm::rotate(model, this->angulo, glm::vec3(0.0, 1.0, 0.0));
  }
  else
  {
    model = glm::rotate(model, angulo, eixo);
  }

  model = glm::scale(model, escala);
  shader->setMat4("model", glm::value_ptr(model));
}

void Malha::desenhar(Material material)
{
  shader->setVec3("ka", material.ambient.r, material.ambient.g, material.ambient.b);
  shader->setVec3("kd", material.diffuse.r, material.diffuse.g, material.diffuse.b);
  shader->setVec3("ks", material.specular.r, material.specular.g, material.specular.b);
  shader->setFloat("q", material.shininess);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, nVertices);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

const string PASTA_RECURSOS = "../../Common/3d-models/suzanne/";
const string CAMINHO_ARQUIVO_OBJ = PASTA_RECURSOS + "SuzanneTriTextured.obj";

struct Geometria
{
  GLuint VAO;
  int verticesCount;
};

struct Vertex
{
  float x, y, z;
};

struct TextureCoord
{
  float s, t;
};

struct Normal
{
  float x, y, z;
};

struct Face
{
  std::vector<int> verticeIndices;
  std::vector<int> indicesCoordTextura;
  std::vector<int> normalIndices;
};

struct ParsedObj
{
  std::vector<float> vertices;
  std::string nomeArquivoMTL;
};

int loadTexture(string mtlPath);
ParsedObj lerArquivoObj(const std::string &mtlPath);
Material readMTLFile(const string &nomeArquivoMTL);
Geometria iniciaGeometria(const std::vector<float> &vertices);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
vector<glm::vec3> geraGrupoPontosControle(string path);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;

bool rotateX,
    rotateY,
    rotateZ;

float translateDistance = 0.0f;

enum Direction
{
  Increase,
  Decrease
};

glm::mat4 calculateTransformations(glm::mat4 model, float angle)
{
  if (rotateX)
  {
    return glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
  }

  if (rotateY)
  {
    return glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
  }

  if (rotateZ)
  {
    return glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
  }

  return model;
}

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s)
{
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s)
{
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s)
{
  return rtrim(ltrim(s));
}

Camera camera;

int main()
{
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

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

  ParsedObj parsedObj = lerArquivoObj(CAMINHO_ARQUIVO_OBJ);
  Material material = readMTLFile(parsedObj.nomeArquivoMTL);

  Shader shader("vertex-shader.vert", "fragment-shader.frag");

  GLuint textureId = loadTexture(material.texturePath);
  Geometria geometry = iniciaGeometria(parsedObj.vertices);

  GLuint VAO = geometry.VAO;
  int verticesCount = geometry.verticesCount;

  glUseProgram(shader.ID);

  camera.inicializar(&shader, width, height);

  Malha suzanne;
  suzanne.inicializar(VAO, verticesCount, &shader, textureId);

  // Definindo as propriedades da fonte de luz
  shader.setVec3("lightPosition", 15.0f, 15.0f, 2.0f);
  shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

  std::vector<glm::vec3> pontosControle = geraGrupoPontosControle("./animacoes/config.txt");

  Bezier bezier;
  bezier.atualizarPontosControle(pontosControle);
  bezier.atualizarShader(&shader);
  bezier.gerarCurva(100);
  int nbCurvaPoints = bezier.getPontosNBCurva();
  int i = 0;

  glEnable(GL_DEPTH_TEST);

  // Loop da aplicação - "game loop"
  while (!glfwWindowShouldClose(window))
  {
    // Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
    glfwPollEvents();

    // Limpa o buffer de cor
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // cor de fundo
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLineWidth(10);
    glPointSize(20);

    camera.sincronizar();

    glm::vec3 pontoNaCurva = bezier.getPontoNaCurva(i);
    suzanne.sincronizarPosicao(pontoNaCurva);
    suzanne.sincronizar();
    suzanne.desenhar(material);

    i = (i + 1) % nbCurvaPoints;

    glfwSwapBuffers(window);
  }

  glDeleteVertexArrays(1, &VAO);
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
    rotateX = key == GLFW_KEY_X;
    rotateY = key == GLFW_KEY_Y;
    rotateZ = key == GLFW_KEY_Z;
  }

  camera.movimentar(window, key, action);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
  camera.rotacionar(window, xpos, ypos);
}

ParsedObj lerArquivoObj(const std::string &filename)
{
  std::ifstream file(filename);
  std::string linha;
  std::vector<Vertex> vertices;
  std::vector<TextureCoord> coordsTextura;

  std::vector<Normal> normais;
  std::vector<Face> faces;
  std::string nomeArquivoMTL;

  while (std::getline(file, linha))
  {
    std::istringstream iss(linha);
    std::string prefixo;
    iss >> prefixo;

    if (prefixo == "v")
    {
      Vertex vertex;
      iss >> vertex.x >> vertex.y >> vertex.z;
      vertices.push_back(vertex);
    }
    else if (prefixo == "vt")
    {
      TextureCoord texCoord;
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
      const Vertex &vertice = vertices[face.verticeIndices[i]];
      const TextureCoord &textura = coordsTextura[face.indicesCoordTextura[i]];
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

  ParsedObj parsedObj;
  parsedObj.vertices = resultado;
  parsedObj.nomeArquivoMTL = nomeArquivoMTL;

  return parsedObj;
}

Material readMTLFile(const string &nomeArquivoMTL)
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
    istringstream iss(linha);
    string palavra;
    iss >> palavra;
    if (palavra == "newmtl")
    {
      iss >> material.name;
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
      // Kd and Ke are the same according to the MTL specification
    }
    else if (palavra == "Kd" || palavra == "Ke")
    {
      float r, g, b;
      iss >> r >> g >> b;
      material.diffuse = glm::vec3(r, g, b);
    }
    else if (palavra == "map_Kd")
    {
      string nomeArquivo;
      iss >> nomeArquivo;
      material.texturePath = PASTA_RECURSOS + trim(nomeArquivo);
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

  // Gera o identificador do VBO
  glGenBuffers(1, &VBO);

  // Vincula o buffer como um buffer de array
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // Envia os dados do array de floats para o buffer da OpenGl
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  // Gera o identificador do VAO (Vertex Array Object)
  glGenVertexArrays(1, &VAO);

  // Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
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

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Desvincula o VAO 
  glBindVertexArray(0);

  // Dividimos por 11 pois cada vértice tem 11 floats (3 coordenadas + 3 cores + 2 texturas + 3 normais)
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

vector<glm::vec3> geraGrupoPontosControle(string arquivo)
{
  vector<glm::vec3> pontosControle;
  string linha;
  ifstream arquivoConfig(arquivo);

  while (getline(arquivoConfig, linha))
  {
    istringstream iss(linha);

    float x, y, z;
    iss >> x >> y >> z;
    pontosControle.push_back(glm::vec3(x, y, z));
  }

  arquivoConfig.close();

  return pontosControle;
}