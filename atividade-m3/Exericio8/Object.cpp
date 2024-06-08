#include "Object.h"
#include "SpriteRenderer.h"

Object::Object()
    : Position(0.0f, 0.0f,0.0f), Size(1.0f, 1.0f), Velocity(0.0f), Color(1.0f), Sprite() { }

Object::Object(glm::vec3 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
    : Position(pos), Size(size), Velocity(velocity), Color(color), Sprite(sprite) { }

void Object::Draw(SpriteRenderer& renderer)
{
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Color);
}