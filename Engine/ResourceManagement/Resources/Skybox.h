#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "ResourceManagement/Manager/SkyboxManager.h"
#include "Texture.h"

#include <array>
#include <vector>
#include <GL/glew.h>

class ComponentCamera;
class Config;
class Metafile;

class Skybox : public Resource
{

public:
	enum class SkyboxFace
	{
		RIGHT = 0,
		LEFT = 1,
		UP = 2,
		DOWN = 3,
		FRONT = 4,
		BACK = 5
	};

	Skybox();
	Skybox(Metafile* resource_metafile);
	~Skybox();

	void Save(Config& config) const;
	void Load(const Config& config);

	void Render(const ComponentCamera & camera) const;

private:
	void GenerateSkyboxCube();
	void GenerateTextures();
	void GenerateTexture(SkyboxFace face);
	void GenerateSkyboxCubeMap();

private:
	std::array<uint32_t, 6> textures_id{};
	std::array<std::shared_ptr<Texture>, 6> textures;

	GLuint cubemap;
	GLuint vao;
	GLuint vbo;
};

namespace ResourceManagement
{
	template<>
	static FileData Binarize(const Skybox& skybox)
	{
		SkyboxManager::Binarize(skybox);
	};

	template<>
	static std::shared_ptr<Skybox> Load(Metafile* metafile, const FileData& resource_data)
	{
		return SkyboxManager::Load(metafile, resource_data);
	}

	template<>
	static FileData Create<Skybox>()
	{
		return SkyboxManager::Create();
	};
}

#endif //_SKYBOX_H_
