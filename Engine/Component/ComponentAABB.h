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
	void Delete() override {};

	void Save(Config& config) const;
	void Load(const Config& config);

	void GenerateBoundingBox();
	void GenerateBoundingBoxFromVertices(const std::vector<ComponentMesh::Vertex> & vertices);

	bool IsEmpty() const;
	std::vector<float> GetVertices() const;

	void ShowComponentWindow(){}

public:
	AABB bounding_box;
	AABB2D bounding_box2D;

};

#endif //_COMPONENTAABB_H_
