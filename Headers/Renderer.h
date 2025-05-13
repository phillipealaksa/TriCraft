#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include "Shaders.h"
#include "Textures.h"
#include "Input.h"
#include "World.h"
#include <mutex>
class Renderer
{
public:
	int Init(); // Init OpenGL
	
	GLFWwindow* CreateWindow(int width, int height, std::string& name);

	Renderer();
private:
	Shaders shaders;
	Textures textures;
	Input input;

	struct VertexTriplet
	{
        float x1, y1, z1, u1, v1;
		float x2, y2, z2, u2, v2;
		float x3, y3, z3, u3, v3;
		bool operator==(const VertexTriplet & other) const
		{
			return x1 == other.x1 && y1 == other.y1 && z1 == other.z1 &&
				   x2 == other.x2 && y2 == other.y2 && z2 == other.z2 && 
				   x3 == other.x3 && y3 == other.y3 && z3 == other.z3;
		}
	};

    struct VertexHash
    {
        std::size_t operator()(VertexTriplet const& vertex) const
        {
            std::size_t h1 = std::hash<float>()(vertex.x1);
            std::size_t h2 = std::hash<float>()(vertex.y1);
            std::size_t h3 = std::hash<float>()(vertex.z1);
            std::size_t h4 = std::hash<float>()(vertex.x2);
            std::size_t h5 = std::hash<float>()(vertex.y2);
            std::size_t h6 = std::hash<float>()(vertex.z2);
            std::size_t h7 = std::hash<float>()(vertex.x3);
            std::size_t h8 = std::hash<float>()(vertex.y3);
            std::size_t h9 = std::hash<float>()(vertex.z3);

			return h1 ^ h2 ^ h3 ^ h4 ^ h5 ^ h6 ^ h7 ^ h8 ^ h9;
        }
    };

	void MouseMoveCallback(GLFWwindow* window, double xposIn, double yposIn);
	void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	

	std::vector<float> CalculateChunkRender(Chunk* chunk);
	void CalculateChunkRenderNTimes(std::vector<Chunk*>& chunkBatch, std::vector<std::vector<float>>& allChunkRenders, std::mutex& renderMutex);
	
	std::vector<float> GetVerticeArrayWithTextureAndProperties(Block block, glm::vec2 chunkPos);
	
};