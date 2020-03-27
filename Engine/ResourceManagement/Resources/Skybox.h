#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "Texture.h"

#include <array>
#include <vector>
#include <GL/glew.h>

class ComponentCamera;
class Metafile;

class Skybox : public Resource
{

public:
	enum class SkyboxFaces
	{
		RIGHT = 0,
		LEFT = 1,
		UP = 2,
		DOWN = 3,
		FRONT = 4,
		BACK = 5
	};

	Skybox() = default;
	Skybox(Metafile* resource_metafile, const std::array<uint32_t, 6>& textures_id);
	~Skybox();

	void Render(const ComponentCamera & camera) const;

private:
	void GenerateSkyboxCube();
	void GenerateTextures(const std::array<uint32_t, 6>& textures_id);
	void GenerateSkyboxCubeMap();

private:
	std::array<std::shared_ptr<Texture>, 6> textures;

	GLuint texture_id;
	GLuint vao;
	GLuint vbo;


};


#endif //_SKYBOX_H_
