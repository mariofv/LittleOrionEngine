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
	LightFrustum(FrustumSubDivision frustum_sub_division) : frustum_sub_division(frustum_sub_division) {};
	~LightFrustum() = default;

	void Update();

	void RenderSubFrustum() const;
	void RenderSubFrustumAABB() const;
	void RenderMeshRenderersAABB() const;

private:
	void UpdateSubPerspectiveFrustum();
	void UpdateSubPerspectiveFrustumAABB();
	void UpdateMeshRenderersAABB();
	void UpdateLightOrthogonalFrustum();

public:
	FrustumSubDivision frustum_sub_division;

	Frustum sub_perspective_frustum;
	AABB sub_perspective_frustum_enclosing_aabb; // Light space
	AABB mesh_renderers_enclosing_aabb; // Light space


	Frustum light_orthogonal_frustum;
};

#endif //__LIGHTFRUSTUM_H__