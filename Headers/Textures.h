#pragma once

#include "World.h"

#include <string>
#include <unordered_map>
#include <vector>

struct TexCoord
{
	int x, y;
};



class Textures
{
public:
	Textures();

	unsigned char* atlas_png;
	TexCoord* GetTextures(BlockType block);
	
	~Textures();
private:
	
	enum TextureType
	{
		EMPTY,
		DIRT,
		DIRT_WITH_GRASS,
		GRASS,
		STONE
	};
	std::string m_AtlasDirectory = "Textures/atlas.png";

	std::unordered_map<BlockType, TexCoord*> m_BlockTextures;
	std::unordered_map<TextureType, TexCoord> m_TextureTypeCoords;

	void SetTextureTypeCoords();
	void SetTextureMap();

	
};