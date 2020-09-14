#ifndef __LIGHTFRUSTUM_H__
#define __LIGHTFRUSTUM_H__

#include <MathGeoLib/MathGeoLib.h>

class LightFrustum
{
public:
	enum class FrustumSubDivision
	{
		NEAR_FRUSTUM,
		MID_FRUSTUM,
		FAR_FRUSTUM
	};

	LightFrustum() = default;
	LightFrustum(FrustumSubDivision frustum_sub_division);
	~LightFrustum() = default;

	void Update();

	void RenderSubFrustum() const;
	void RenderSubFrustumAABB() const;
	void RenderMeshRenderersAABB() const;
	void RenderLightFrustum() const;

private:
	void UpdateSubPerspectiveFrustum();
	void UpdateSubPerspectiveFrustumAABB();
	void UpdateMeshRenderersAABB();
	void UpdateLightOrthogonalFrustum();

public:
	FrustumSubDivision frustum_sub_division;

	Frustum sub_perspective_frustum;
	float3 sub_perspective_frustum_render_color;

	AABB sub_perspective_frustum_enclosing_aabb; // Light space
	float3 sub_perspective_frustum_enclosing_aabb_render_color;

	AABB mesh_renderers_enclosing_aabb; // Light space
	float3 mesh_renderers_enclosing_aabb_render_color;

	Frustum light_orthogonal_frustum;
	float3 light_orthogonal_frustum_render_color;
};

#endif //__LIGHTFRUSTUM_H__