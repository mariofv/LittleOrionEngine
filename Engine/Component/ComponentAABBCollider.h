#ifndef _COMPONENTAABBCOLLIDER_H_
#define _COMPONENTAABBCOLLIDER_H_

#include "Component.h"
#include "MathGeoLib.h"
#include "ComponentMesh.h"

#include "GL/glew.h"


class ComponentAABBCollider : public Component
{
public:
	enum class CollisionState
	{
		OUTSIDE,
		INTSIDE,
		INTERSECT,
	};

	ComponentAABBCollider();
	ComponentAABBCollider(GameObject * owner);
	~ComponentAABBCollider();

	void Enable() override;
	void Disable() override;
	void Update() override;

	void Render(const GLuint shader_program) const;

	ComponentAABBCollider::CollisionState CheckAABBCollision(const AABB& reference_AABB) const;

	void GenerateBoundingBox();
	void GenerateBoundingBoxFromVertices(const std::vector<ComponentMesh::Vertex> & vertices);
	void GenerateBoundingBoxFromFrustum(const Frustum & frustum);

	void ShowComponentWindow(){}
	AABB bounding_box;

};

#endif //_COMPONENTAABBCOLLIDER_H_
