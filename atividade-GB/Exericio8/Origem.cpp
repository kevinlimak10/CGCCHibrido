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
  void inicializar(Shader *shader, int largura, int altura, glm::vec3 cameraFrente = glm::vec3(0.0, 0.0, -1.0), glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0), glm::vec3 cameraCima = glm::vec3(0.0, 1.0, 0.0), float precisao = 0.03, float inclinacao = 0.0, float direcao = -90.0);
  void rotacionar(GLFWwindow *window, double posX, double posY);
  void movimentar(GLFWwindow *window, int tecla, int acao, float velocidade = 0.08);
  void sincronizar();

protected:
  Shader *shader;
  bool prioridadeMouse;
  float atualX, atualY, inclinacao, direcao;
  float precisao;
  glm::vec3 cameraFrente, cameraPos, cameraCima;
};

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

class Curva
{
public:
  Curva() {}
  inline void atualizarPontosControle(vector<glm::vec3> pontosControle) { this->pontosControle = pontosControle; }
  void atualizarShader(Shader *shader);
  void gerarCurva(int pontosPorSegmento);
  void desenharCurva(glm::vec4 color);
  int getPontosNBCurva() { return pontosCurva.size(); }
  glm::vec3 getPontoNaCurva(int i) { return pontosCurva[i]; }

protected:
  vector<glm::vec3> pontosControle;
  vector<glm::vec3> pontosCurva;
  glm::mat4 M; // Matriz de base
  GLuint VAO;
  Shader *shader;
};

void Curva::atualizarShader(Shader *shader)
{
  this->shader = shader;
  shader->Use();
}

void Curva::desenharCurva(glm::vec4 color)
{
  shader->setVec4("finalColor", color.r, color.g, color.b, color.a);

  glBindVertexArray(VAO);
  glDrawArrays(GL_LINE_STRIP, 0, pontosCurva.size());
  glBindVertexArray(0);
}

class Bezier : public Curva
{
public:
  Bezier();
  void gerarCurva(int pontosPorSegmento);
};

Bezier::Bezier()
{
  M = glm::mat4(
      -1, 3, -3, 1,
      3, -6, 3, 0,
      -3, 3, 0, 0,
      1, 0, 0, 0);
}

void Bezier::gerarCurva(int pontosPorSegmento)
{
  float passo = 1.0 / (float)pontosPorSegmento;

  float t = 0;

  int nPontosControle = pontosControle.size();

  for (int i = 0; i < nPontosControle - 3; i += 3)
  {

    for (float t = 0.0; t <= 1.0; t += passo)
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
  glBindVertexArray(VAO);

  // posição (x, y, z)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Desvincula o VAO 
  glBindVertexArray(0);
}

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

class Malha
{
public:
  Malha() {}
  ~Malha() {}
  void inicializar(GLuint VAO, int nVertices, Shader *shader, GLuint textureID, glm::vec3 posicao = glm::vec3(0.0, 0.0, 0.0), glm::vec3 escala = glm::vec3(0.5, 0.5, 0.5), float angulo = 0.0, glm::vec3 eixo = glm::vec3(0.0, 0.0, 1.0));
  void sincronizar();
  void desenhar(Material material);
  void sincronizarPosicao(glm::vec3 posicao);
  void atualizarRotacionarY(int deveRotacionarY);
  void atualizarScale(int isScale);
  void atualizarTransalate(int isTranslate);
  GLuint VAO;

protected:
  int nVertices;

  // Informações sobre as transformações a serem aplicadas no objeto
  glm::vec3 posicao;
  glm::vec3 escala;
  float angulo;
  glm::vec3 eixo;

  // Referência (endereço) do shader
  Shader *shader;

  GLuint textureID;

  int deveRotacionarY = 0;
  int deveTransladar = 0;
  int deveEscalar = 0;
};

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
  this->deveRotacionarY = 0;
  this->deveTransladar = 0;
  this->deveEscalar = 0;
}

void Malha::sincronizarPosicao(glm::vec3 posicao)
{
  this->posicao = posicao;
}

void Malha::atualizarScale(int isScale)
{
  this->deveEscalar = isScale;
}

void Malha::atualizarRotacionarY(int deveRotacionarY)
{
    this->deveRotacionarY = deveRotacionarY;
}
void Malha::atualizarTransalate(int isTranslate)
{
    this->deveTransladar = isTranslate;
}

void Malha::sincronizar()
{
  glm::mat4 model = glm::mat4(1);
  model = glm::translate(model, posicao);

  if (deveRotacionarY > 0)
  {
        this->angulo += 0.001;
        model = glm::rotate(model, this->angulo, glm::vec3(0.0, 1.0, 0.0));
  }
  else if(deveRotacionarY < 0)
  {
        this->angulo -= 0.001;
        model = glm::rotate(model, this->angulo, glm::vec3(0.0, 1.0, 0.0));
  }
  else {
      model = glm::rotate(model, angulo, eixo);
  }

  if (deveEscalar > 0) {
      this->escala += 0.001;
      model = glm::scale(model, this->escala);
  }
  else if (deveEscalar < 0) {
      this->escala -= 0.001;
      model = glm::scale(model, this->escala);
  }
  else {
      model = glm::scale(model, this->escala);
  }

  if (deveTransladar > 0) {
      this->posicao += 0.001;
      model = glm::translate(model, this->posicao);
  }
  else if(deveTransladar < 0){
      this->posicao -= 0.001;
      model = glm::translate(model, this->posicao);
  }
  else {
      model = glm::translate(model, this->posicao);
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

bool pressedMouseButton = false;

struct Face
{
  vector<int> indicesVertices;
  vector<int> indicesCoordsTextura;
  vector<int> normalIndices;
};

struct Vertice
{
  float x, y, z;
};

struct CoordTextura
{
  float s, t;
};

struct Normal
{
  float x, y, z;
};

struct ParsedObj
{
  vector<float> vertices;
  string nomeArquivoMTL;
};

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
    cout << "Failed to load texture" << endl;
  }

  stbi_image_free(data);

  glBindTexture(GL_TEXTURE_2D, 0);

  return texID;
}

ParsedObj lerArquivoObj(const string &nomeArquivo)
{
  ifstream file(nomeArquivo);
  string linha;
  vector<Vertice> vertices;
  vector<CoordTextura> coordsTextura;

  vector<Normal> normais;
  vector<Face> faces;
  string nomeArquivoMTL;

  while (getline(file, linha))
  {
    istringstream iss(linha);
    string prefixo;
    iss >> prefixo;

    if (prefixo == "v")
    {
      Vertice vertex;
      iss >> vertex.x >> vertex.y >> vertex.z;
      vertices.push_back(vertex);
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
      vector<string> stringsVertices;
      string verticeString;

      while (iss >> verticeString)
      {
        stringsVertices.push_back(verticeString);
      }

      if (stringsVertices.size() >= 3)
      {
        for (size_t i = 1; i < stringsVertices.size() - 1; ++i)
        {
          const string &v0String = stringsVertices[0];
          const string &v1String = stringsVertices[i];
          const string &v2String = stringsVertices[i + 1];

          istringstream vss0(v0String);
          istringstream vss1(v1String);
          istringstream vss2(v2String);

          string v0IndiceString, v0CoordTexString, v0IndiceStringNormal;
          string v1IndiceString, v1CoordTexString, v1IndiceStringNormal;
          string v2IndiceString, v2CoordTexString, v2IndiceStringNormal;

          getline(vss0, v0IndiceString, '/');
          getline(vss0, v0CoordTexString, '/');
          getline(vss0, v0IndiceStringNormal, '/');

          getline(vss1, v1IndiceString, '/');
          getline(vss1, v1CoordTexString, '/');
          getline(vss1, v1IndiceStringNormal, '/');

          getline(vss2, v2IndiceString, '/');
          getline(vss2, v2CoordTexString, '/');
          getline(vss2, v2IndiceStringNormal, '/');

          face.indicesVertices.push_back(stoi(v0IndiceString) - 1);
          face.indicesCoordsTextura.push_back(stoi(v0CoordTexString) - 1);
          face.normalIndices.push_back(stoi(v0IndiceStringNormal) - 1);

          face.indicesVertices.push_back(stoi(v1IndiceString) - 1);
          face.indicesCoordsTextura.push_back(stoi(v1CoordTexString) - 1);
          face.normalIndices.push_back(stoi(v1IndiceStringNormal) - 1);

          face.indicesVertices.push_back(stoi(v2IndiceString) - 1);
          face.indicesCoordsTextura.push_back(stoi(v2CoordTexString) - 1);
          face.normalIndices.push_back(stoi(v2IndiceStringNormal) - 1);
        }

        faces.push_back(face);
      }
    }
  }

  vector<float> resultado;

  for (const Face &face : faces)
  {
    for (size_t i = 0; i < face.indicesVertices.size(); ++i)
    {
      const Vertice &vertice = vertices[face.indicesVertices[i]];
      const CoordTextura &textura = coordsTextura[face.indicesCoordsTextura[i]];
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

vector<Material> lerArquivoMTL(const string &pastaRecursos, const string &nomeArquivoMTL)
{
  vector<Material> materiais;
  ifstream file(pastaRecursos + nomeArquivoMTL);

  if (!file.is_open())
  {
    cout << "Failed to open file: " << nomeArquivoMTL << endl;
    return materiais;
  }

  Material materialAtual;
  string line;

  while (getline(file, line))
  {
    istringstream iss(line);
    string palavra;
    iss >> palavra;

    if (palavra == "newmtl")
    {
      // Encontrou outro material, entao armazena o anterior
      if (!materialAtual.name.empty())
      {
        materiais.push_back(materialAtual);
      }

      materialAtual = Material();
      iss >> materialAtual.name;
    }
    else if (palavra == "Ka")
    {
      float r, g, b;
      iss >> r >> g >> b;
      materialAtual.ambient = glm::vec3(r, g, b);
    }
    else if (palavra == "Ks")
    {
      float r, g, b;
      iss >> r >> g >> b;
      materialAtual.specular = glm::vec3(r, g, b);
    }
    else if (palavra == "Kd" || palavra == "Ke")
    {
      float r, g, b;
      iss >> r >> g >> b;
      materialAtual.diffuse = glm::vec3(r, g, b);
    }
    else if (palavra == "map_Kd")
    {
      string fileName;
      iss >> fileName;
      materialAtual.texturePath = pastaRecursos + trim(fileName);
    }
    else if (palavra == "Ns")
    {
      float shininess;
      iss >> shininess;
      materialAtual.shininess = shininess;
    }
  }

  // Adiciona ultimo material
  if (!materialAtual.name.empty())
  {
    materiais.push_back(materialAtual);
  }

  file.close();
  return materiais;
}

vector<glm::vec3> gerarConjuntoPontosControle(string fileName)
{
  vector<glm::vec3> pontosControle;
  string line;
  ifstream configFile("./animacoes/" + fileName + ".txt");

  while (getline(configFile, line))
  {
    istringstream iss(line);

    float x, y, z;
    iss >> x >> y >> z;
    pontosControle.push_back(glm::vec3(x, y, z));
  }

  configFile.close();

  return pontosControle;
}

const string PASTA_RECURSOS = "./recursos/";
const string BOLA_OBJ_FILE_PATH = PASTA_RECURSOS + "CuboTextured.obj";
const string SUZANNE_OBJ_FILE_PATH = PASTA_RECURSOS + "SuzanneTriTextured.obj";
const string PLANETA_OBJ_FILE_PATH = PASTA_RECURSOS + "planeta.obj";

struct Geometria
{
  GLuint VAO;
  int verticesCount;
};

Geometria iniciaGeometria(const std::vector<float> &vertices);
vector<glm::vec3> gerarConjuntoPontosControle(string path);

const GLuint WIDTH = 1000, HEIGHT = 1000;

Camera camera;
vector<Malha*> objetosSelecionados;
int objetoSelecionado;
int moveTranslateObjectSelect;
int moveScaleObjectSelect;
int moveRoteteSelect;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);

  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
      objetoSelecionado = (objetoSelecionado + 1) % objetosSelecionados.size();
  }
  if (key == GLFW_KEY_O && action == GLFW_PRESS) {
      moveTranslateObjectSelect = 1;
  }
  if (key == GLFW_KEY_P && action == GLFW_PRESS) {
      moveTranslateObjectSelect = -1;
  }
  if (key == GLFW_KEY_L && action == GLFW_PRESS) {
      moveScaleObjectSelect = 1;
  }
  if (key == GLFW_KEY_K && action == GLFW_PRESS) {
      moveScaleObjectSelect = -1;
  }
  if (key == GLFW_KEY_M && action == GLFW_PRESS) {
      moveRoteteSelect = 1;
  }
  if (key == GLFW_KEY_N && action == GLFW_PRESS) {
      moveRoteteSelect = -1;
  }
  camera.movimentar(window, key, action);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
  camera.rotacionar(window, xpos, ypos);
}


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
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

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

  Shader shader("./vertex-shader.vert", "./fragment-shader.frag");

  glUseProgram(shader.ID);

  camera.inicializar(&shader, width, height);

  ParsedObj parsedBolaObj = lerArquivoObj(BOLA_OBJ_FILE_PATH);
  vector<Material> bolaMaterials = lerArquivoMTL(PASTA_RECURSOS, parsedBolaObj.nomeArquivoMTL);
  Material bolaMaterial = bolaMaterials[0];
  GLuint bolaTexturaId = loadTexture(bolaMaterial.texturePath);
  Geometria bolaGeometria = iniciaGeometria(parsedBolaObj.vertices);
  GLuint BOLA_VAO = bolaGeometria.VAO;
  int bolaNumVertices = bolaGeometria.verticesCount;

  Malha bola;
  bola.inicializar(BOLA_VAO, bolaNumVertices, &shader, bolaTexturaId, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f));
  bola.atualizarRotacionarY(true);
  std::string gluintString = std::to_string(BOLA_VAO);
  cout << "bola VAO:" + gluintString << endl;
  objetosSelecionados.push_back(&bola);


  ParsedObj parsedSuzanneObj = lerArquivoObj(SUZANNE_OBJ_FILE_PATH);
  vector<Material> suzanneMateriais = lerArquivoMTL(PASTA_RECURSOS, parsedSuzanneObj.nomeArquivoMTL);
  Material suzanneMaterial = suzanneMateriais[0];
  GLuint suzanneTexturaId = loadTexture(suzanneMaterial.texturePath);
  Geometria suzanneGeometria = iniciaGeometria(parsedSuzanneObj.vertices);

  GLuint SUZANNE_VAO = suzanneGeometria.VAO;
  std::string gluintStringSuzane = std::to_string(SUZANNE_VAO);
  int suzanneNumVertices = suzanneGeometria.verticesCount;

  Malha suzanne;
  suzanne.inicializar(SUZANNE_VAO, suzanneNumVertices, &shader, suzanneTexturaId, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.15f, 0.15f, 0.15f));
  suzanne.atualizarRotacionarY(true);
  cout << "Suzanne VAO:" + gluintStringSuzane << endl;
  objetosSelecionados.push_back(&suzanne);

  ParsedObj parsedPlanetaObj = lerArquivoObj(PLANETA_OBJ_FILE_PATH);
  vector<Material> planetMateriais = lerArquivoMTL(PASTA_RECURSOS, parsedPlanetaObj.nomeArquivoMTL);
  Material planetMaterial = planetMateriais[0];
  GLuint planetTextureId = loadTexture(planetMaterial.texturePath);
  Geometria planetGeometria = iniciaGeometria(parsedPlanetaObj.vertices);
  GLuint PLANET_VAO = planetGeometria.VAO;
  int planetNumVertices = planetGeometria.verticesCount;

  Malha planet;
  planet.inicializar(PLANET_VAO, planetNumVertices, &shader, planetTextureId, glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.15f, 0.15f, 0.15f));
  planet.atualizarRotacionarY(false);
  std::string gluintStringPlanet = std::to_string(PLANET_VAO);
  cout << "Planet VAO:" + gluintStringPlanet << endl;
  objetosSelecionados.push_back(&planet);

  // Definindo as propriedades da fonte de luz
  shader.setVec3("lightPosition", 15.0f, 15.0f, 2.0f);
  shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

  std::vector<glm::vec3> pontosControle = gerarConjuntoPontosControle("config");

  Bezier bezier;
  bezier.atualizarPontosControle(pontosControle);
  bezier.atualizarShader(&shader);
  bezier.gerarCurva(100);
  int nbCurvaPoints = bezier.getPontosNBCurva();
  int pontoNaCurvaAtual = 0;

  glEnable(GL_DEPTH_TEST);

  // Loop da aplicação - "game loop"
  while (!glfwWindowShouldClose(window))
  {
    // Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
    glfwPollEvents();

    // Limpa o buffer de cor
    glClearColor(0.5f, 0.5f, 1.0f, 1.0f); // cor de fundo
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLineWidth(10);
    glPointSize(20);
    cout << objetosSelecionados[objetoSelecionado]->VAO << endl;
    if (moveTranslateObjectSelect == 1) {
        objetosSelecionados[objetoSelecionado]->atualizarTransalate(1);
        moveTranslateObjectSelect = false;
        //objetosSelecionados[objetoSelecionado]->atualizarTransalate(0);
    }
    if (moveTranslateObjectSelect == -1) {
        objetosSelecionados[objetoSelecionado]->atualizarTransalate(-1);
        moveTranslateObjectSelect = false;
        //objetosSelecionados[objetoSelecionado]->atualizarTransalate(0);
    }
    
    if (moveScaleObjectSelect == 1) {
        objetosSelecionados[objetoSelecionado]->atualizarScale(1);
        moveScaleObjectSelect = false;
        //objetosSelecionados[objetoSelecionado]->atualizarScale(0);
    }
    if (moveScaleObjectSelect == -1) {
        objetosSelecionados[objetoSelecionado]->atualizarScale(-1);
        moveScaleObjectSelect = false;
        //objetosSelecionados[objetoSelecionado]->atualizarScale(0);
    }
    
    if (moveRoteteSelect == 1) {
        objetosSelecionados[objetoSelecionado]->atualizarRotacionarY(1);
        moveRoteteSelect = false;
        //objetosSelecionados[objetoSelecionado]->atualizarRotacionarY(0);
    }
    if (moveRoteteSelect == -1) {
        objetosSelecionados[objetoSelecionado]->atualizarRotacionarY(-1);
        moveRoteteSelect = false;
        //objetosSelecionados[objetoSelecionado]->atualizarRotacionarY(0);
    }

    camera.sincronizar();

    glm::vec3 pontoNaCurva = bezier.getPontoNaCurva(pontoNaCurvaAtual);
    bola.sincronizarPosicao(pontoNaCurva);

    bola.sincronizar();
    bola.desenhar(bolaMaterial);

    suzanne.sincronizar();
    suzanne.desenhar(suzanneMaterial);

    planet.sincronizar();
    planet.desenhar(planetMaterial);

    pontoNaCurvaAtual = (pontoNaCurvaAtual + 1) % nbCurvaPoints;

    glfwSwapBuffers(window);
    objetosSelecionados[objetoSelecionado]->atualizarScale(0);
    objetosSelecionados[objetoSelecionado]->atualizarRotacionarY(0);
    objetosSelecionados[objetoSelecionado]->atualizarTransalate(0);
  }

  glDeleteVertexArrays(1, &BOLA_VAO);
  glDeleteVertexArrays(1, &SUZANNE_VAO);
  glDeleteVertexArrays(1, &PLANET_VAO);
  glfwTerminate();
  return 0;
}

Geometria iniciaGeometria(const std::vector<float> &vertices)
{
  GLuint VBO, VAO;

  // Gera do identificador do VBO
  glGenBuffers(1, &VBO);

  // Vincula o buffer como um buffer de array
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // Envia os dados do array de floats para o buffer da OpenGl
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  // Gera o identificador do VAO (Vertex Array Object)
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

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
  glBindVertexArray(0);

  // Dividimos por 11 pois cada vértice tem 11 floats (3 coordenadas + 3 cores + 2 texturas + 3 normais)
  int verticesCount = vertices.size() / 11;

  return {
      VAO,
      verticesCount,
  };
}
