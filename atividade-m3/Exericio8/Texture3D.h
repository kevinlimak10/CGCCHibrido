#pragma once
#include <vector>
#include "Texture2D.h"
#include <string>
#include "Vertice.cpp"
#include "CoordenadaTextura.cpp"
#include "Face.cpp"
#include <iostream>
#include <assert.h>
#include <fstream>
#include <sstream>


class Texture3D
{
public:
    std::vector<Texture2D> Textures;
    std::vector<Vertice> Vertices;
    std::vector<CoordenadaTextura> CoordsTextura;
    std::vector<Face> Faces;

    // Constructor
    Texture3D(const std::vector<Texture2D>& textures,
        const std::vector<Vertice>& vertices,
        const std::vector<CoordenadaTextura>& coordsTextura,
        const std::vector<Face>& faces)
        : Textures(textures),
        Vertices(vertices),
        CoordsTextura(coordsTextura),
        Faces(faces)
    {}
    // constructor (sets default texture modes)
    Texture3D();

    Texture3D parseArquivoTexture3D(const std::string& nomeArquivo);
};