#include "Balzier.h"

Balzier::Balzier()
{
    M = glm::mat4(
        -1, 3, -3, 1,
        3, -6, 3, 0,
        -3, 3, 0, 0,
        1, 0, 0, 0);
}

void Balzier::gerarCurva(int pontosPorSegmento)
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Desvincula o VAO 
    glBindVertexArray(0);
}