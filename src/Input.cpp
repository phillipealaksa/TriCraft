#include "Input.h"

void Input::KeyPress(GLFWwindow* window)
{
	for (auto key : KeyPressFunctions)
	{
		if (glfwGetKey(window, key.first) == GLFW_PRESS)
		{
			for (auto func : key.second)
			{
				func();
			}
		}
	}
}

void Input::MouseMoveCallback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (m_FirstMouse)
	{
		m_LastX = xpos;
		m_LastY = ypos;
		m_FirstMouse = false;
	}

	float xoffset = xpos - m_LastX;
	float yoffset = m_LastY - ypos;

	m_LastX = xpos;
	m_LastY = ypos;

	for (auto func : MouseMoveFunctions)
	{
		func(xoffset, yoffset);
	}
}

void Input::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	for (auto func : MouseScrollFunctions)
	{
		func(static_cast<float>(yoffset));
	}
}

Input::Input() {

}