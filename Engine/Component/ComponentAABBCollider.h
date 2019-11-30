#ifndef _COMPONENTAABBCOLLIDER_H_
#define _COMPONENTAABBCOLLIDER_H_

#include "Component.h"
#include "MathGeoLib.h"
#include "ComponentMesh.h"

#include "GL/glew.h"


class ComponentAABBCollider : public Component
{
public:
	ComponentAABBCollider();
	ComponentAABBCollider(GameObject * owner);
	~ComponentAABBCollider();

	void Enable() override;
	void Disable() override;
	void Update() override;

	void Render(const GLuint shader_program) const;

	void GenerateBoundingBox();
	AABB GenerateBoundingBoxFromVertex(const std::vector<ComponentMesh::Vertex> & vertices);

	void ShowComponentWindow(){}
	AABB bounding_box;

};

#endif //_COMPONENTAABBCOLLIDER_H_
