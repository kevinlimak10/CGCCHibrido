#include <string>
#include <glm/gtc/type_ptr.hpp>


struct Material
{
    std::string name;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    std::string texturePath;
    float shininess;
    int textureId;
};