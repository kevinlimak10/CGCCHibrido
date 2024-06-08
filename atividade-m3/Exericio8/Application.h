#include <glm/glm.hpp>


const glm::vec2 PLAYER_SIZE(60.0f, 60.0f);

class Application
{
public:
	void Init();
	Application(unsigned int width, unsigned int height);
	~Application();
	bool         Keys[1024];
	unsigned int Width, Height;
	void Render();
	void ProcessInput(float dt);
	void Update(float dt);
};

