// Object.h
#ifndef OBJECT_H
#define OBJECT_H

#include "ObjectInterface.h"
#include <vector>
#include "CoordenadaTextura.cpp"
#include "Face.cpp"

class Object : public ObjectInterface
{
public:
    glm::vec3 Position;
    glm::vec3 Color;
    glm::vec2 Size, Velocity;
    std::vector<Vertice> Vertices;
    std::vector<CoordenadaTextura> coordsTextura;
    std::vector<Face> faces;
    Texture2D Sprite;

    Object(glm::vec3 position, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f), std::vector<Vertice> vertices);


    Object(); 

    void Move(const glm::vec3& delta) override
    {
        Position += delta;
    }

    void Resize(float scale) override
    {
        // Assuming uniform scaling for simplicity
        // Implement specific logic as needed
        // E.g., Position *= scale; or adjusting Sprite dimensions
    }

    virtual void Draw(SpriteRenderer& renderer);
};

#endif // OBJECT_H