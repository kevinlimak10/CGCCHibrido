#include <vector>
// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "include/shader.h"
#include "Curve.h"

class Balzier : public Curve
{
public:
	Balzier();
	void gerarCurva(int pontosPorSegmento);
};
