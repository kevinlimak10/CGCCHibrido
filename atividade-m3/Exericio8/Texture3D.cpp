#include <iostream>

#include "Texture3D.h"


Texture3D::Texture3D()

{
    glGenTextures(1, &this->ID);
}

void Texture3D::Generate(unsigned int width, unsigned int height, unsigned char* data, int nrChannels)
{
    this->Width = width;
    this->Height = height;
    // Criando as Texturas
    glBindTexture(GL_TEXTURE_2D, this->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
    // setando as textures wrap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture3D::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, this->ID);
}