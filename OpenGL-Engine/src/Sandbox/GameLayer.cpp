#include "Sandbox/GameLayer.h"
#include <glad/glad.h>

GameLayer::GameLayer()
{

}

void GameLayer::OnUpdate(float dt)
{
	glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
