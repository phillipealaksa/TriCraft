#include "World.h"
#include <numbers>

World::World()
{

	int i = 0;
	glm::vec3 rotation;
	rotation.x = 0.0f;
	rotation.y = 0.0f;
	rotation.z = glm::radians(0.0f);
	for (int xd = -renderDistance; xd < renderDistance; xd++)
	{
		int yRange = renderDistance - 1 - std::abs(xd);
		for (int yd = -yRange; yd <= yRange; yd++)
		{
			chunks[i] = new Chunk();
			chunks[i]->Position = glm::vec2(xd, yd);
			chunks[i]->ID = i;
			for (int x = 0; x < 16; x++)
			{
				for (int z = 0; z < 16; z++)
				{
					float heightNoise = noise.octaveNoise((x + xd * 16) * 0.05, (z + yd * 16) * 0.05, 6, 0.5, 2.0);
					int terrainHeight = static_cast<int>(heightNoise * 20.0) + 32; // scale & offset to fit in 64 height

					for (int y = 0; y < 64; y++)
					{
						glm::vec3 position(x, y, z);

						if (y < terrainHeight - 1)
						{
							chunks[i]->Blocks[x][y][z] = Block(position, BlockType::STONE, BlockShape::CUBE, rotation);
						}
						else if (y == terrainHeight - 1)
						{
							chunks[i]->Blocks[x][y][z] = Block(position, BlockType::GRASS, BlockShape::CUBE, rotation);
						}
						else
						{
							chunks[i]->Blocks[x][y][z] = Block(position, BlockType::EMPTY, BlockShape::CUBE, rotation);
						}
					}
				}
			}
			i++;
		}
	}
}

void World::FreeChunks()
{
	for (int i = 0; i < 1 + (2 * renderDistance) * (renderDistance - 1); i++)
	{
		delete chunks[i];
	}
	chunks.clear();
}