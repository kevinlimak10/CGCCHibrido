#include "Curve.h"

void Curve::atualizarShader(Shader* shader)
{
	this->shader = shader;
	shader->Use();
}

void Curve::desenharCurva(glm::vec4 color)
{
	shader->setVec4("finalColor", color.r, color.g, color.b, color.a);

	glBindVertexArray(VAO);
	glDrawArrays(GL_LINE_STRIP, 0, pontosCurva.size());
	glBindVertexArray(0);
}
