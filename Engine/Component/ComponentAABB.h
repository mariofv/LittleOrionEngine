#ifndef _COMPONENTAABB_H_
#define _COMPONENTAABB_H_

#include "Component.h"
#include "ResourceManagement/Resources/Mesh.h"

#include <MathGeoLib.h>
#include <GL/glew.h>

class ComponentParticleSystem;
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
	ComponentAABB(GameObject* owner);
	~ComponentAABB() = default;

	//Copy and move
	ComponentAABB(const ComponentAABB& component_to_copy) = default;
	ComponentAABB(ComponentAABB&& component_to_move) = default;

	ComponentAABB & operator=(const ComponentAABB& component_to_copy) = default;
	ComponentAABB & operator=(ComponentAABB&& component_to_move) = default;
	void CopyTo(Component* component_to_copy) const override;

	void Delete() override {};
	void GenerateBoundingBox();

	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

	bool IsEmpty() const;

private:
	void GenerateBoundingBoxFromVertices(const std::vector<Mesh::Vertex>& vertices);
	void GenerateBoundingBoxFromParticleSystem(const ComponentParticleSystem& particle_system);
	void GenerateGlobalBoundingBox();
	Component* Clone(GameObject* owner, bool original_prefab) override;
public:
	AABB bounding_box;
	AABB global_bounding_box;
	AABB original_box;
	AABB2D bounding_box2D;

};

#endif //_COMPONENTAABB_H_
