#pragma once

#include <GLFW/glfw3.h>

#include <vector>
#include <unordered_map>
#include <functional>

class Input
{
public:
	std::unordered_map<uint16_t, std::vector<std::function<void()>>> KeyPressFunctions;
	std::vector<std::function<void(float, float)>> MouseMoveFunctions;
	std::vector<std::function<void(float)>> MouseScrollFunctions;

	void KeyPress(GLFWwindow* window);
	void MouseMoveCallback(GLFWwindow* window, double xposIn, double yposIn);
	void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	Input();
private:
	bool m_FirstMouse = true;
	float m_LastX, m_LastY;
};