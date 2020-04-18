#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_

#include "ResourceManagement/Resources/Mesh.h"
#include "ResourceManagement/Resources/Texture.h"

#include <string>
#include <MathGeoLib.h>
#include <GL/glew.h>

class Billboard
{
public:
	Billboard(const std::string& texture_path, float width, float height);
	~Billboard();

	void Render(const float3& position) const;
	

private:
	std::shared_ptr<Mesh> billboard_quad;

	float width = 5.f;
	float height = 5.f;
	std::shared_ptr<Texture> billboard_texture = nullptr;

	float vertices[12] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	unsigned int indices[6] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	unsigned int EBO;

};

#endif //_BILLBOARD_H_