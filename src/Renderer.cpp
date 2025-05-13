#include "Renderer.h"
#include "Camera.h"

#include <iostream>
#include <string>
#include <thread>

#define GLEW_STATIC
#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h> // GLEW
#include <GLFW/glfw3.h> // GLFW

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


int Renderer::Init()
{
	glewExperimental = true;
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW\n";
		return -1;
	}
	glEnable(GL_DEPTH_TEST);

	return 1;
}

GLFWwindow* Renderer::CreateWindow(int width, int height, std::string& name)
{
	float deltaTime = 0.0f;	// time between current frame and last frame
	float lastFrame = 0.0f;

	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
	float lastX = width / 2.0f;
	float lastY = height / 2.0f;
	bool firstMouse = true;





	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);




	GLFWwindow* window;
	window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to open GLFW window";
		glfwTerminate();
		return nullptr;
	}




	glfwMakeContextCurrent(window);
	glewExperimental = true;


	glfwSetWindowUserPointer(window, &input);

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y)
		{
			Input* inp = static_cast<Input*>(glfwGetWindowUserPointer(window));
			inp->MouseMoveCallback(window, x, y);
		});

	glfwSetScrollCallback(window, [](GLFWwindow* window, double x, double y)
		{
			Input* inp = static_cast<Input*>(glfwGetWindowUserPointer(window));
			inp->MouseScrollCallback(window, x, y);
		});

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW\n";
	}
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);



	// Load and create a texture
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, textures.atlas_png);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glGenerateMipmap(GL_TEXTURE_2D);

	shaders.SetUpShaders();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_BLEND);



	shaders.UseShaders();

	input.KeyPressFunctions[GLFW_KEY_SPACE].push_back([&]()
		{
			camera.UpdatePosition(CameraMovement::FORWARD, deltaTime);
		});

	input.KeyPressFunctions[GLFW_KEY_RIGHT].push_back([&]()
		{
			camera.UpdatePosition(CameraMovement::RIGHT, deltaTime);
		});

	input.KeyPressFunctions[GLFW_KEY_DOWN].push_back([&]()
		{
			camera.UpdatePosition(CameraMovement::BACKWARD, deltaTime);
		});

	input.KeyPressFunctions[GLFW_KEY_LEFT].push_back([&]()
		{
			camera.UpdatePosition(CameraMovement::LEFT, deltaTime);
		});

	input.KeyPressFunctions[GLFW_KEY_UP].push_back([&]()
		{
			camera.UpdatePosition(CameraMovement::UP, deltaTime);
		});

	input.KeyPressFunctions[GLFW_KEY_LEFT_ALT].push_back([&]()
		{
			camera.UpdatePosition(CameraMovement::DOWN, deltaTime);
		});

	input.MouseMoveFunctions.push_back([&](float x, float y)
		{
			camera.UpdateDirection(x, y);
		});

	input.MouseScrollFunctions.push_back([&](float y)
		{
			camera.UpdateZoom(y);
		});
	glBindTexture(GL_TEXTURE_2D, textureID);
	int n = 20;
	// Main render loop
	World w;
	const int MAX_THREADS = std::thread::hardware_concurrency() - 1;
	std::vector<std::thread> threads;
	threads.reserve(MAX_THREADS);

	std::vector<Chunk*> chunks;
	int numChunks = 1 + (2 * w.renderDistance) * (w.renderDistance - 1);
	int t = 0;

	std::vector<std::vector<float>> allChunkRenders;
	std::mutex renderMutex;

	int chunksPerThread = std::ceil((float)numChunks / MAX_THREADS);
	int currentIndex = 0;

	while (currentIndex < numChunks)
	{
		std::vector<Chunk*> chunkBatch;
		for (int i = 0; i < chunksPerThread && currentIndex < numChunks; ++i, ++currentIndex)
		{
			chunkBatch.push_back(w.chunks[currentIndex]);
		}

		// Capture necessary things by value/reference
		threads.emplace_back([this, chunkBatch, &allChunkRenders, &renderMutex]() mutable {
			CalculateChunkRenderNTimes(chunkBatch, allChunkRenders, renderMutex);
			});
	}

	for (auto& thread : threads)
	{
		thread.join();
	}
	w.FreeChunks();
	std::vector<float> allChunkRendersMerged;
	for (const auto& chunkRender : allChunkRenders)
	{
		allChunkRendersMerged.insert(allChunkRendersMerged.end(), chunkRender.begin(), chunkRender.end());
	}

	// Now all chunk renders are merged into allChunkRenders, you can process them

	// Create buffers and array objects
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);


	glBindVertexArray(VAO);


	// Bind and set vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, allChunkRendersMerged.size() * sizeof(float), &allChunkRendersMerged[0], GL_STATIC_DRAW);

	// Set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Texture coordinates attribute pointer
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode


	while (!glfwWindowShouldClose(window)) {
		// Input


		std::cout << 1 / deltaTime << "FPS\n";
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		input.KeyPress(window);
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 1000.0f);
		glUniformMatrix4fv(glGetUniformLocation(shaders.ShaderProgramID, "projection"), 1, GL_FALSE, &projection[0][0]);

		// camera view transformation
		glm::mat4 view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shaders.ShaderProgramID, "view"), 1, GL_FALSE, &view[0][0]);
		// render boxes

		glm::mat4 model = glm::mat4(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(shaders.ShaderProgramID, "model"), 1, GL_FALSE, &model[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, allChunkRendersMerged.size() / 5);

		// Swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaders.ShaderProgramID);

	glfwTerminate();
	return window;
}

void Renderer::MouseMoveCallback(GLFWwindow* window, double xposIn, double yposIn)
{
	input.MouseMoveCallback(window, xposIn, yposIn);
}

void Renderer::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	input.MouseScrollCallback(window, xoffset, yoffset);
}

std::vector<float> Renderer::CalculateChunkRender(Chunk* chunk)
{
	int xt = 16;
	int yt = 64;
	int zt = 16;
	Block currentBlock;
	std::vector<float> vertices;
	std::vector<float> chunkRender;
	for (int y = 0; y < yt; y++)
	{
		for (int x = 0; x < xt; x++)
		{
			for (int z = 0; z < zt; z++)
			{
				currentBlock = chunk->Blocks[x][y][z];
				if (currentBlock.Type == BlockType::EMPTY)
				{
					continue;
				}
				vertices = GetVerticeArrayWithTextureAndProperties(currentBlock, chunk->Position);
				if (currentBlock.Shape == BlockShape::CUBE)
				{

					for (int i = 0; i < 6; i++)
					{
						if (i == 0)
						{
							if (z > 0)
							{
								if (chunk->Blocks[x][y][z - 1].Type == BlockType::EMPTY)
								{
									chunkRender.insert(chunkRender.end(), vertices.begin() + i * 30, vertices.begin() + (i + 1) * 30);
								}
							}
							else if (z == 0)
							{
								chunkRender.insert(chunkRender.end(), vertices.begin() + i * 30, vertices.begin() + (i + 1) * 30);
							}
						}
						else if (i == 1)
						{
							if (z < zt - 1)
							{
								if (chunk->Blocks[x][y][z + 1].Type == BlockType::EMPTY)
								{
									chunkRender.insert(chunkRender.end(), vertices.begin() + i * 30, vertices.begin() + (i + 1) * 30);
								}
							}
							else if (z == zt - 1)
							{
								chunkRender.insert(chunkRender.end(), vertices.begin() + i * 30, vertices.begin() + (i + 1) * 30);
							}
						}
						else if (i == 2)
						{
							if (x > 0)
							{
								if (chunk->Blocks[x - 1][y][z].Type == BlockType::EMPTY)
								{
									chunkRender.insert(chunkRender.end(), vertices.begin() + i * 30, vertices.begin() + (i + 1) * 30);
								}
							}
							else if (x == 0)
							{
								chunkRender.insert(chunkRender.end(), vertices.begin() + i * 30, vertices.begin() + (i + 1) * 30);
							}
						}
						else if (i == 3)
						{
							if (x < xt - 1)
							{
								if (chunk->Blocks[x + 1][y][z].Type == BlockType::EMPTY)
								{
									chunkRender.insert(chunkRender.end(), vertices.begin() + i * 30, vertices.begin() + (i + 1) * 30);
								}
							}
							else if (x == xt - 1)
							{
								chunkRender.insert(chunkRender.end(), vertices.begin() + i * 30, vertices.begin() + (i + 1) * 30);
							}
						}
						else if (i == 4)
						{
							if (y > 0)
							{
								if (chunk->Blocks[x][y - 1][z].Type == BlockType::EMPTY)
								{
									chunkRender.insert(chunkRender.end(), vertices.begin() + i * 30, vertices.begin() + (i + 1) * 30);
								}
							}
							else if (y == 0)
							{
								chunkRender.insert(chunkRender.end(), vertices.begin() + i * 30, vertices.begin() + (i + 1) * 30);
							}
						}
						else
						{
							if (y < yt - 1)
							{
								if (chunk->Blocks[x][y + 1][z].Type == BlockType::EMPTY)
								{
									chunkRender.insert(chunkRender.end(), vertices.begin() + i * 30, vertices.begin() + (i + 1) * 30);
								}
							}
							else if (y == yt - 1)
							{
								chunkRender.insert(chunkRender.end(), vertices.begin() + i * 30, vertices.begin() + (i + 1) * 30);
							}
						}
					}
				}
				else
				{
					chunkRender.insert(chunkRender.end(), vertices.begin(), vertices.end());
				}


			}
		}
	}
	return chunkRender;
}


std::vector<float> Renderer::GetVerticeArrayWithTextureAndProperties(Block block, glm::vec2 chunkPos)
{
	TexCoord* txc = textures.GetTextures(block.Type);

	float u1[7];
	float v1[7];
	float u2[7];
	float v2[7];

	for (int i = 0; i < 7; i++)
	{
		u1[i] = txc[i].x * (1.0f / 32);
		v1[i] = (31 - txc[i].y) * (1.0f / 32);
		u2[i] = (txc[i].x + 1) * (1.0f / 32);
		v2[i] = (31 - txc[i].y + 1) * (1.0f / 32);
	}

	float sqrt2 = (float)sqrt(2);

	glm::quat rotation = glm::quat(block.Rotation);

	if (block.Shape == BlockShape::CUBE)
	{
		std::vector<float> cubeVertices = {
			//back
			0.0f, 0.0f, 0.0f, u1[0], v1[0],
			1.0f, 0.0f, 0.0f, u2[0], v1[0],
			1.0f, 1.0f, 0.0f, u2[0], v2[0],
			1.0f, 1.0f, 0.0f, u2[0], v2[0],
			0.0f, 1.0f, 0.0f, u1[0], v2[0],
			0.0f, 0.0f, 0.0f, u1[0], v1[0],
			//front			 
			0.0f, 0.0f, 1.0f, u1[1], v1[1],
			1.0f, 0.0f, 1.0f, u2[1], v1[1],
			1.0f, 1.0f, 1.0f, u2[1], v2[1],
			1.0f, 1.0f, 1.0f, u2[1], v2[1],
			0.0f, 1.0f, 1.0f, u1[1], v2[1],
			0.0f, 0.0f, 1.0f, u1[1], v1[1],
			//left			   		
			0.0f, 1.0f, 1.0f, u2[2], v2[2],
			0.0f, 1.0f, 0.0f, u1[2], v2[2],
			0.0f, 0.0f, 0.0f, u1[2], v1[2],
			0.0f, 0.0f, 0.0f, u1[2], v1[2],
			0.0f, 0.0f, 1.0f, u2[2], v1[2],
			0.0f, 1.0f, 1.0f, u2[2], v2[2],
			//right		
			1.0f, 1.0f, 1.0f, u2[3], v2[3],
			1.0f, 1.0f, 0.0f, u1[3], v2[3],
			1.0f, 0.0f, 0.0f, u1[3], v1[3],
			1.0f, 0.0f, 0.0f, u1[3], v1[3],
			1.0f, 0.0f, 1.0f, u2[3], v1[3],
			1.0f, 1.0f, 1.0f, u2[3], v2[3],
			//bottom		   	
			0.0f, 0.0f, 0.0f, u1[4], v2[4],
			1.0f, 0.0f, 0.0f, u2[4], v2[4],
			1.0f, 0.0f, 1.0f, u2[4], v1[4],
			1.0f, 0.0f, 1.0f, u2[4], v1[4],
			0.0f, 0.0f, 1.0f, u1[4], v1[4],
			0.0f, 0.0f, 0.0f, u1[4], v2[4],
			//top			   		 
			0.0f, 1.0f, 0.0f, u1[5], v2[5],
			1.0f, 1.0f, 0.0f, u2[5], v2[5],
			1.0f, 1.0f, 1.0f, u2[5], v1[5],
			1.0f, 1.0f, 1.0f, u2[5], v1[5],
			0.0f, 1.0f, 1.0f, u1[5], v1[5],
			0.0f, 1.0f, 0.0f, u1[5], v2[5]
		};

		for (int i = 0; i < 180; i += 5)
		{
			glm::vec3 vertex(cubeVertices[i], cubeVertices[i + 1], cubeVertices[i + 2]);
			glm::vec3 u(rotation.x, rotation.y, rotation.z);
			float s = rotation.w;

			vertex = glm::rotate(rotation, vertex - glm::vec3(0.5f, 0.5f, 0.5f));
			cubeVertices[i] = vertex.x + block.Position.x + (chunkPos.x * 16) + 0.5f;
			cubeVertices[i + 1] = vertex.y + block.Position.y + 0.5f;
			cubeVertices[i + 2] = vertex.z + block.Position.z + (chunkPos.y * 16) + 0.5f;
		}
		return cubeVertices;
	}
	else if (block.Shape == BlockShape::PRISM)
	{
		std::vector<float> prismVertices = {
			//back
			0.0f, 0.0f, 0.0f, u1[0], v1[0],
			1.0f, 0.0f, 0.0f, u2[0], v1[0],
			1.0f, 1.0f, 0.0f, u2[0], v2[0],
			1.0f, 1.0f, 0.0f, u2[0], v2[0],
			0.0f, 1.0f, 0.0f, u1[0], v2[0],
			0.0f, 0.0f, 0.0f, u1[0], v1[0],
			//left					 
			0.0f, 0.0f, 0.0f, u2[2], v1[2],
			0.0f, 1.0f, 0.0f, u2[2], v2[2],
			0.0f, 0.0f, 1.0f, u1[2], v1[2],
			//right
			1.0f, 0.0f, 0.0f, u1[3], v1[3],
			1.0f, 1.0f, 0.0f, u1[3], v2[3],
			1.0f, 0.0f, 1.0f, u2[3], v1[3],
			//bottom				 
			0.0f, 0.0f, 0.0f, u1[4], v2[4],
			1.0f, 0.0f, 0.0f, u2[4], v2[4],
			1.0f, 0.0f, 1.0f, u2[4], v1[4],
			1.0f, 0.0f, 1.0f, u2[4], v1[4],
			0.0f, 0.0f, 1.0f, u1[4], v1[4],
			0.0f, 0.0f, 0.0f, u1[4], v2[4],
			//inside bottom			 
			0.0f, 0.0f, 1.0f, u1[6], v1[6],
			1.0f, 0.0f, 1.0f, u2[6], v1[6],
			1.0f, 1.0f - sqrt2 / 2, sqrt2 / 2, u2[6], v1[6] + ((sqrt2 - 1) / 32),
			1.0f, 1.0f - sqrt2 / 2, sqrt2 / 2, u2[6], v1[6] + ((sqrt2 - 1) / 32),
			0.0f, 0.0f, 1.0f, u1[6], v1[6],
			0.0f, 1.0f - sqrt2 / 2, sqrt2 / 2, u1[6], v1[6] + ((sqrt2 - 1) / 32),
			// inside top			 
			0.0f, 1.0f - sqrt2 / 2, sqrt2 / 2, u1[6], v1[6],
			1.0f, 1.0f - sqrt2 / 2, sqrt2 / 2, u2[6], v1[6],
			1.0f, 1.0f, 0.0f, u2[6], v2[6],
			1.0f, 1.0f, 0.0f, u2[6], v2[6],
			0.0f, 1.0f - sqrt2 / 2, sqrt2 / 2, u1[6], v1[6],
			0.0f, 1.0f, 0.0f, u1[6], v2[6],
		};

		for (int i = 0; i < 150; i += 5)
		{
			glm::vec3 vertex(prismVertices[i], prismVertices[i + 1], prismVertices[i + 2]);
			glm::vec3 u(rotation.x, rotation.y, rotation.z);
			float s = rotation.w;

			vertex = glm::rotate(rotation, vertex - glm::vec3(0.5f, 0.5f, 0.5f));
			prismVertices[i] = vertex.x + block.Position.x + (chunkPos.x * 16) + 0.5f;
			prismVertices[i + 1] = vertex.y + block.Position.y + 0.5f;
			prismVertices[i + 2] = vertex.z + block.Position.z + (chunkPos.y * 16) + 0.5f;
		}
		return prismVertices;
	}
}

void Renderer::CalculateChunkRenderNTimes(std::vector<Chunk*>& chunkBatch, std::vector<std::vector<float>>& allChunkRenders, std::mutex& renderMutex)
{
	std::vector<float> threadRenderData;

	for (Chunk* chunk : chunkBatch)
	{
		std::vector<float> chunkData = CalculateChunkRender(chunk);
		threadRenderData.insert(threadRenderData.end(), chunkData.begin(), chunkData.end());
	}

	// Safely push thread's full render data to shared vector
	std::lock_guard<std::mutex> lock(renderMutex);
	allChunkRenders.push_back(std::move(threadRenderData));
}

Renderer::Renderer()
{

}
