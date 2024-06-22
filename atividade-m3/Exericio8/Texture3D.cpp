#include <string>
#include "Vertice.cpp"
#include "CoordenadaTextura.cpp"
#include "Face.cpp"
#include <iostream>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <vector>
#include "Texture3D.h"



Texture3D Texture3D::parseArquivoTexture3D(const std::string& nomeArquivo) {
    std::ifstream file(nomeArquivo);
    std::string linha;
    std::string nomeArquivoMTL;
    std::vector<Vertice> vertices;
    std::vector<CoordenadaTextura> coordsTextura;
    std::vector<Face> faces;

    while (std::getline(file, linha))
    {
        std::istringstream iss(linha);
        std::string prefixo;
        iss >> prefixo;

        if (prefixo == "vt")
        {
            CoordenadaTextura coordTex;
            iss >> coordTex.s >> coordTex.t;
            coordsTextura.push_back(coordTex);
        }
        else if (prefixo == "mtllib")
        {
            iss >> nomeArquivoMTL;
        }
        else if (prefixo == "v")
        {

            Vertice vertice;
            iss >> vertice.x >> vertice.y >> vertice.z;
            vertices.push_back(vertice);
        }
        else if (prefixo == "f")
        {
            Face face;
            std::string verticeStr;

            while (iss >> verticeStr)
            {
                std::istringstream vss(verticeStr);
                std::string verticeIndiceString, indiceCoordTexturaString, normalIndexString;

                std::getline(vss, verticeIndiceString, '/');
                std::getline(vss, indiceCoordTexturaString, '/');

                face.verticeIndices.push_back(std::stoi(verticeIndiceString) - 1);
                if (!indiceCoordTexturaString.empty())
                    face.indicesCoordTextura.push_back(std::stoi(indiceCoordTexturaString) - 1);
            }

            faces.push_back(face);
        }
    }
    std::vector<Texture2D> textures;
    return Texture3D::Texture3D(textures, vertices, coordsTextura, faces);
}