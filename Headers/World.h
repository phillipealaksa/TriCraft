#pragma once

#include <glm/glm.hpp>
#include "PerlinNoise.h"

#include <vector>

enum BlockFaces
{
	BACK = 0,
	FRONT = 1,
	LEFT = 2,
	RIGHT = 3,
	BOTTOM = 4,
	TOP = 5,
	INSIDE = 6
};

enum BlockType
{
	EMPTY,
	GRASS,
	STONE
};

enum BlockShape
{
	CUBE,
	PRISM
};

struct Block
{
	glm::vec3 Position;
	BlockType Type;
	BlockShape Shape;
	glm::vec3 Rotation;
};

struct Chunk
{
	glm::vec2 Position;
	Block Blocks[16][64][16];
	int ID;
};

class World
{
public:
	static constexpr int renderDistance = 10;
	void FreeChunks();
	std::vector<Chunk*> chunks = std::vector<Chunk*>(1 + (2 * renderDistance) * (renderDistance - 1));
	World();
private:
	PerlinNoise noise;
	

};