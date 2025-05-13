#include "Game.h"

#define GLEW_STATIC
#include "Renderer.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int Game::Init()
{
	if (renderer.Init() == -1)
	{
		return -1;
	}
	return 1;
}

void Game::Run()
{
	window = renderer.CreateWindow(m_windowWidth, m_windowHeight, m_windowName);

}

Game::Game()
{

}