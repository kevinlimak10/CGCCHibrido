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
#include "CameraRecord.h"
#include "Mesh.h"
#include "Balzier.h"



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

struct Geometria
{
  GLuint VAO;
  int verticesCount;
};

Geometria iniciaGeometria(const std::vector<float> &vertices);
vector<glm::vec3> gerarConjuntoPontosControle(string path);

const GLuint WIDTH = 1000, HEIGHT = 1000;

CameraRecord camera;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);

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

  Mesh bola;
  bola.inicializar(BOLA_VAO, bolaNumVertices, &shader, bolaTexturaId, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f));
  bola.atualizarRotacionarY(true);

  ParsedObj parsedSuzanneObj = lerArquivoObj(SUZANNE_OBJ_FILE_PATH);
  vector<Material> suzanneMateriais = lerArquivoMTL(PASTA_RECURSOS, parsedSuzanneObj.nomeArquivoMTL);
  Material suzanneMaterial = suzanneMateriais[0];
  GLuint suzanneTexturaId = loadTexture(suzanneMaterial.texturePath);
  Geometria suzanneGeometria = iniciaGeometria(parsedSuzanneObj.vertices);
  GLuint SUZANNE_VAO = suzanneGeometria.VAO;
  int suzanneNumVertices = suzanneGeometria.verticesCount;

  Mesh suzanne;
  suzanne.inicializar(SUZANNE_VAO, suzanneNumVertices, &shader, suzanneTexturaId, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.15f, 0.15f, 0.15f));
  suzanne.atualizarRotacionarY(true);

  // Definindo as propriedades da fonte de luz
  shader.setVec3("lightPosition", 15.0f, 15.0f, 2.0f);
  shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

  std::vector<glm::vec3> pontosControle = gerarConjuntoPontosControle("config");

  Balzier bezier;
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

    camera.sincronizar();

    glm::vec3 pontoNaCurva = bezier.getPontoNaCurva(pontoNaCurvaAtual);
    bola.sincronizarPosicao(pontoNaCurva);

    bola.sincronizar();
    bola.desenhar(bolaMaterial);

    suzanne.sincronizar();
    suzanne.desenhar(suzanneMaterial);

    pontoNaCurvaAtual = (pontoNaCurvaAtual + 1) % nbCurvaPoints;

    glfwSwapBuffers(window);
  }

  glDeleteVertexArrays(1, &BOLA_VAO);
  glDeleteVertexArrays(1, &SUZANNE_VAO);
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
