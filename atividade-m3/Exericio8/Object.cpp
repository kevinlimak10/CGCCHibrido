#include "Object.h"
#include "SpriteRenderer.h"

Object::Object()
    : Position(0.0f, 0.0f,0.0f), Size(1.0f, 1.0f), Velocity(0.0f), Color(1.0f) { }

Object::Object(glm::vec3 pos, Texture3D sprite, glm::vec3 color, glm::vec2 velocity, std::vector<Vertice> vertices)
    : Position(pos), Velocity(velocity), Texture3D(color), Sprite(sprite) , Vertices(vertices) { }

void Object::Draw(SpriteRenderer& renderer)
{
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Color,this->Vertices);
}