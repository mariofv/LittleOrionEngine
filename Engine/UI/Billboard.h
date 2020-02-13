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

};

#endif //_BILLBOARD_H_