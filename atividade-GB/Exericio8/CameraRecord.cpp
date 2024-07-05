#include "CameraRecord.h"




void CameraRecord::inicializar(Shader* shader, int largura, int altura, glm::vec3 cameraFrente, glm::vec3 cameraPos, glm::vec3 cameraCima, float precisao, float inclinacao, float direcao)
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

void CameraRecord::rotacionar(GLFWwindow* window, double posX, double posY)
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

void CameraRecord::movimentar(GLFWwindow* window, int tecla, int acao, float velocidade)
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

void CameraRecord::sincronizar()
{
    // Atualizar a posição e orientação da câmera
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFrente, cameraCima);
    shader->setMat4("view", glm::value_ptr(view));

    // Atualizar o shader com a posição da câmera
    shader->setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);
}
