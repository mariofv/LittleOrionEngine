#ifndef _MODULEDEBUGDRAW_H_
#define _MODULEDEBUGDRAW_H_

#ifndef ENGINE_EXPORTS
#define ENGINE_EXPORTS
#endif

#include "Module.h"

#include <GL/glew.h>
#include <MathGeoLib.h>
#include <queue>

class ComponentCamera;
class ComponentMeshRenderer;
class GameObject;
class Grid;
class IDebugDrawOpenGLImplementation;

class ModuleDebugDraw : public Module
{
public:
	struct OutlineEntity
	{
		ComponentMeshRenderer* mesh_renderer = nullptr;
		float4 color = float4::one;
	};

	ModuleDebugDraw() = default;
	~ModuleDebugDraw() = default;

	bool Init() override;
	bool CleanUp() override;

	void Render(float width, float height, const float4x4& projection_view_matrix);

	ENGINE_API void RenderSingleAABB(AABB& aabb, const float3& color = float3::one) const;
	ENGINE_API void RenderBoundingBoxes(const float3& color = float3::one) const;
	ENGINE_API void RenderGlobalBoundingBoxes(const float3& color = float3::one) const;

	ENGINE_API void RenderNavMesh(ComponentCamera& cam) const;

	ENGINE_API void RenderPathfinding() const;
	ENGINE_API void RenderQuadTree(const float3& color = float3::one) const;
	ENGINE_API void RenderOcTree(const float3& color = float3::one) const;
	ENGINE_API void RenderAABBTree(const float3& color = float3::one) const;
	
	void RenderSelectedGameObjectHelpers() const;
	
	void RenderSelectedGameObjectsOutline();
	void RenderGrid() const;
	void RenderBillboards() const;
	void RenderTangentsAndBitangents() const;
	void RenderSphere(float3& position, float3& color, float radius) const;
	
	void RenderRectTransform(const GameObject* rect_owner) const;
	ENGINE_API void RenderLine(const float3& a, const float3& b, const float3& color = float3::one) const;
	ENGINE_API void RenderPoint(const float3& point, float size= 1.f, const float3& color = float3::one) const;
	ENGINE_API void RenderCircle(const float3& center, float radius= 1.f, const float3& normal = float3::unitY, const float3& color = float3::one) const;
	ENGINE_API void RenderSphere(const float3& center, float radius= 1.f, const float3& color = float3::one) const;
	ENGINE_API void RenderBox(const float3 points[8], const float3& color = float3::one);
	ENGINE_API void RenderAxis(const float4x4& axis_space, float size = 1.f, float length = 10.f, const float3& color = float3::one);
	ENGINE_API void RenderPerspectiveFrustum(const float4x4& inverse_clip_matrix, const float3& color = float3::one) const;
	ENGINE_API void RenderOrtographicFrustum(const float3 points[8], const float3& color = float3::one) const;
	ENGINE_API void RenderOutline(ComponentMeshRenderer* mesh_renderer, const float4& color = float4::one);

	ENGINE_API void RenderPhysics() const;

private:
	void RenderCameraFrustum() const;
	void RenderParticleSystem() const;
	void RenderLightGizmo() const;
	void RenderBones(GameObject * game_object) const;
	void RenderOutlineInternal(ComponentMeshRenderer* mesh_renderer, const float4& color = float4::one) const;

private:
	static IDebugDrawOpenGLImplementation* dd_interface_implementation;

	std::queue<OutlineEntity> outline_entities;

	const float MIN_MAGNITUDE_ORDER_GRID = 0;
	const float MAX_MAGNITUDE_ORDER_GRID = 20;

	Grid* grid = nullptr;
};

#endif //_MODULEDEBUGDRAW_H_
