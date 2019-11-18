#ifndef _MODEL_H_
#define _MODEL_H_

#include "Mesh.h"
#include "BoundingBox.h"

#include "MathGeoLib.h"
#include <GL/glew.h>

class Model
{
public:
	Model() = default;
	Model(const std::vector<Mesh*> meshes, const unsigned int num_materials, GLuint* material_textures);
	~Model();

	void Render(GLuint shader_program) const;

public:
	void ComputeBoundingBox();

public:
	float3 translation;
	float3x3 rotation;
	float3 scale;
	
	BoundingBox *bounding_box = nullptr;

private:
	std::vector<Mesh*> meshes;
	GLuint *material_textures = nullptr;
	unsigned int num_materials;

};

#endif // _MODEL_H_

