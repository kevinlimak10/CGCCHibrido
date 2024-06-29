#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <vector>
#include "Vertice.cpp"
#include "CoordenadaTextura.cpp"
#include "Face.cpp"

// Texture2D is able to store and configure a texture in OpenGL.
// It also hosts utility functions for easy management.
class Texture3D
{
public:
	unsigned int ID;
	std::vector<Vertice> Vertices;
	std::vector<CoordenadaTextura> coordsTextura;
	std::vector<Face> faces;

	Generate(unsigned char* data, int width, int height, int nrChannels)
};

#endif
#pragma once