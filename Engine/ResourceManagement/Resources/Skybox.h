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
	Skybox(uint32_t uuid);
	~Skybox();

	void Save(Config& config) const;
	void Load(const Config& config);

	void Render(const ComponentCamera & camera) const;

private:
	void GenerateSkyboxCube();
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
	static FileData Binarize<Skybox>(Resource* skybox)
	{
		SkyboxManager::Binarize(static_cast<Skybox*>(skybox));
	};

	template<>
	static std::shared_ptr<Skybox> Load(uint32_t uuid, const FileData& resource_data, bool async)
	{
		return SkyboxManager::Load(uuid, resource_data);
	}

	template<>
	static FileData Create<Skybox>()
	{
		return SkyboxManager::Create();
	};
}

#endif //_SKYBOX_H_
