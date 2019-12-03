#ifndef _COMPONENTAABB_H_
#define _COMPONENTAABB_H_

#include "Component.h"
#include "MathGeoLib.h"
#include "ComponentMesh.h"

#include "GL/glew.h"

class ComponentCamera;

class ComponentAABB : public Component
{
public:
	enum class CollisionState
	{
		OUTSIDE,
		INSIDE,
		INTERSECT,
	};

	ComponentAABB();
	ComponentAABB(GameObject * owner);
	~ComponentAABB();

	void Enable() override;
	void Disable() override;
	void Update() override;

	void Render(const ComponentCamera &camera, const GLuint shader_program) const;

	void GenerateBoundingBox();
	void GenerateBoundingBoxFromVertices(const std::vector<ComponentMesh::Vertex> & vertices);

	void ShowComponentWindow(){}
	AABB bounding_box;

};

#endif //_COMPONENTAABB_H_
