#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include "Renderer.h"

class Game
{
public:
    Game();
	int Init();
	void Run();

private:
	int m_windowWidth = 1080;
	int m_windowHeight = 720;
	std::string m_windowName = "TriCraft";
	GLFWwindow* window = nullptr;
	Renderer renderer;
	
};