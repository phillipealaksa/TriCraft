#include "Textures.h"

#include <string>
#include <filesystem>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Textures::Textures()
{
	stbi_set_flip_vertically_on_load(true);
	
	int width, height, channels;
	
	atlas_png = stbi_load(m_AtlasDirectory.c_str(), &width, &height, &channels, 0);

	int pixel_count = width * height;
	for (int i = 0; i < pixel_count; i++) {
		atlas_png[i * 4 + 3] = 255; // Set alpha to fully opaque
	}


	SetTextureTypeCoords();
	SetTextureMap();
}

TexCoord* Textures::GetTextures(BlockType block)
{
	return m_BlockTextures[block];
}

void Textures::SetTextureTypeCoords()
{
	m_TextureTypeCoords[TextureType::DIRT]					= TexCoord(0, 0);
	m_TextureTypeCoords[TextureType::DIRT_WITH_GRASS]		= TexCoord(1, 0);
	m_TextureTypeCoords[TextureType::GRASS]					= TexCoord(2, 0);
	m_TextureTypeCoords[TextureType::STONE]					= TexCoord(3, 0);
}

void Textures::SetTextureMap()
{
	TexCoord faces[7];

	faces[BlockFaces::BACK]   = m_TextureTypeCoords[TextureType::DIRT_WITH_GRASS];
	faces[BlockFaces::FRONT]  = m_TextureTypeCoords[TextureType::DIRT_WITH_GRASS];
	faces[BlockFaces::LEFT]   = m_TextureTypeCoords[TextureType::DIRT_WITH_GRASS];
	faces[BlockFaces::RIGHT]  = m_TextureTypeCoords[TextureType::DIRT_WITH_GRASS];
	faces[BlockFaces::BOTTOM] = m_TextureTypeCoords[TextureType::DIRT];
	faces[BlockFaces::TOP]    = m_TextureTypeCoords[TextureType::GRASS];
	faces[BlockFaces::INSIDE] = m_TextureTypeCoords[TextureType::DIRT];
	m_BlockTextures[BlockType::GRASS] = new TexCoord[7];
	for (int i = 0; i < 7; i++)
	{
		m_BlockTextures[BlockType::GRASS][i] = faces[i];
	}

	faces[BlockFaces::BACK]   = m_TextureTypeCoords[TextureType::STONE];
	faces[BlockFaces::FRONT]  = m_TextureTypeCoords[TextureType::STONE];
	faces[BlockFaces::LEFT]   = m_TextureTypeCoords[TextureType::STONE];
	faces[BlockFaces::RIGHT]  = m_TextureTypeCoords[TextureType::STONE];
	faces[BlockFaces::BOTTOM] = m_TextureTypeCoords[TextureType::STONE];
	faces[BlockFaces::TOP]    = m_TextureTypeCoords[TextureType::STONE];
	faces[BlockFaces::INSIDE] = m_TextureTypeCoords[TextureType::STONE];
	m_BlockTextures[BlockType::STONE] = new TexCoord[7];
	for (int i = 0; i < 7; i++)
	{
		m_BlockTextures[BlockType::STONE][i] = faces[i];
	}
}

Textures::~Textures()
{
	for (auto& i : m_BlockTextures)
	{
		delete i.second;
	}
}