#include "Application.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include <stdio.h>
#include <iostream>
#include "Object.h"
#include <GLFW/glfw3.h>
#include "Shader.h"


SpriteRenderer* Renderer;
Object* Player;

Application::Application(unsigned int width, unsigned int height)
    : Keys(), Width(width), Height(height)
{}

Application::~Application()
{
    delete Renderer;
}

void Application::Init() {
	ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");

    // configura shaders.
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Shader myShader;
    myShader = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(myShader);

    // Carrega texturas.
    ResourceManager::LoadTexture("textures/fundo_mar.png", false, "background");

    glm::vec3 playerPos = glm::vec3(10.0f, 1.5f, 1.0f);
    Player = new Object(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("life"));

}

/// <summary>
/// Realiza a atualização da tela recebendo o tempo por parametro
/// </summary>
/// <param name="dt">dt tempo de atualização entre o ultimo frame e o atual</param>
void Application::Update(float dt)
{

}

/// <summary>
/// Faz a verificação do input recebido do usuario
/// </summary>
/// <param name="dt">dt tempo de atualização entre o ultimo frame e o atual</param>
void Application::ProcessInput(float dt)
{
    // Movimento do player.
    if (this->Keys[GLFW_KEY_UP])
    {
        std::cout << "Apertou para cima";
    }
    if (this->Keys[GLFW_KEY_DOWN])
    {
        std::cout << "Apertou para baixo";
    }
}

void Application::Render()
{
    //Renderizando o background
    Texture2D background;
    background = ResourceManager::GetTexture("background");
    Renderer->DrawSprite(background, glm::vec3(0.0f, 0.0f,0.0f), glm::vec2(this->Width, this->Height));
    Player->Draw(*Renderer);
    //Renderizando player.
}
