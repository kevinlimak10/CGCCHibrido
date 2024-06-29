#ifndef OBJECT_INTERFACE_H
#define OBJECT_INTERFACE_H
#include <glm/glm.hpp>

#include "Texture3D.h"
class SpriteRenderer;

class ObjectInterface
{
public:
    virtual ~ObjectInterface() = default;

    // Pure virtual methods
    virtual void Move(const glm::vec3& delta) = 0;
    virtual void Resize(float scale) = 0;
    virtual void Draw(SpriteRenderer& renderer) = 0;
};
#endif // OBJECT_INTERFACE_H