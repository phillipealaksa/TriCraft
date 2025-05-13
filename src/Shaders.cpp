#include "Shaders.h"

#define GLEW_STATIC
#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <string>

void Shaders::SetUpShaders()
{
	std::string VertexPath = "Shaders/Vertex.shader";
	std::string FragmentPath = "Shaders/Fragment.shader";
	std::string VertexCode;
	std::string FragmentCode;
	std::string line;
	std::ifstream ifs;

	const char* cVertexCode;
	const char* cFragmentCode;

	ifs.open(VertexPath);
	while (std::getline(ifs, line))
	{
		VertexCode += line + '\n';
	}
	cVertexCode = VertexCode.c_str();
	
	ifs.close();
	
	ifs.open(FragmentPath);
	while (std::getline(ifs, line))
	{
		FragmentCode += line + '\n';
	}
	
	cFragmentCode = FragmentCode.c_str();
	ifs.close();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &cVertexCode, NULL);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &cFragmentCode, NULL);
	glCompileShader(fragmentShader);

	ShaderProgramID = glCreateProgram();
	glAttachShader(ShaderProgramID, vertexShader);
	glAttachShader(ShaderProgramID, fragmentShader);
	glLinkProgram(ShaderProgramID);

	GLint success;
	GLchar infoLog[1024];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
		std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << "vertex" << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
	}
	glGetProgramiv(vertexShader, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(vertexShader, 1024, NULL, infoLog);
		std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << "vertex" << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
	}
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
		std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << "frag" << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
	}
	glGetProgramiv(fragmentShader, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(fragmentShader, 1024, NULL, infoLog);
		std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << "frag" << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}


void Shaders::UseShaders()
{
	glUseProgram(ShaderProgramID);
}