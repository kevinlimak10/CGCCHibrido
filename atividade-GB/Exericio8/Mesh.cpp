#include "Mesh.h"
#include "Origem.cpp"



void Mesh::inicializar(GLuint VAO, int nVertices, Shader* shader, GLuint textureID, glm::vec3 posicao, glm::vec3 escala, float angulo, glm::vec3 eixo)
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

void Mesh::sincronizarPosicao(glm::vec3 posicao)
{
    this->posicao = posicao;
}

void Mesh::atualizarRotacionarY(bool deveRotacionarY)
{
    this->deveRotacionarY = deveRotacionarY;
}

void Mesh::sincronizar()
{
    glm::mat4 model = glm::mat4(1);
    model = glm::translate(model, posicao);

    if (this->deveRotacionarY)
    {
        this->angulo += 0.001;
        model = glm::rotate(model, this->angulo, glm::vec3(0.0, 1.0, 0.0));
    }
    else
    {
        model = glm::rotate(model, this->angulo, this->eixo);
    }

    model = glm::scale(model, this->escala);
    shader->setMat4("model", glm::value_ptr(model));
}

void Mesh::desenhar(Material material)
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
