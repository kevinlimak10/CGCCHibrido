#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Texture3D.h"
#include "Shader.h"
#include "Vertice.cpp"


class SpriteRenderer
{
public:
    // Construtor
    SpriteRenderer(Shader& shader);
    // Destructor
    ~SpriteRenderer();
    // Renders a defined quad textured with given sprite
    void DrawSprite(Texture3D& texture, glm::vec3 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), glm::vec3 color = glm::vec3(1.0f), std::vector<Vertice> vertices);
private:

    Shader       shader;
    unsigned int quadVAO;

    void initRenderData();
};

#endif