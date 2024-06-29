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
	// texture image dimensions
	unsigned int Width, Height; // width and height of loaded image in pixels
	// texture Format
	unsigned int Internal_Format; // format of texture object
	unsigned int Image_Format; // format of loaded image
	unsigned int Wrap_S; // wrapping mode on S axis
	unsigned int Wrap_T; // wrapping mode on T axis
	unsigned int Filter_Min; // filtering mode if texture pixels < screen pixels
	unsigned int Filter_Max; // filtering mode if texture pixels > screen pixels
	std::vector<Vertice> Vertices;
	std::vector<CoordenadaTextura> coordsTextura;
	std::vector<Face> faces;

	// generates texture from image data
	void Generate(unsigned int width, unsigned int height, unsigned char* data, int nrChannels);
};

#endif
#pragma once