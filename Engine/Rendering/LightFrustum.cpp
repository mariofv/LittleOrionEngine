#include "LightFrustum.h"

#include "Component/ComponentMeshRenderer.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleDebugDraw.h"
#include "Module/ModuleLight.h"
#include "Module/ModuleSpacePartitioning.h"
#include "Module/ModuleRender.h"

void LightFrustum::Update()
{
	UpdateSubPerspectiveFrustum();
	UpdateSubPerspectiveFrustumAABB();
	UpdateMeshRenderersAABB();
}

void LightFrustum::UpdateSubPerspectiveFrustum()
{
	sub_perspective_frustum.type = FrustumType::PerspectiveFrustum;
	sub_perspective_frustum.pos = App->cameras->main_camera->owner->transform.GetGlobalTranslation();
	sub_perspective_frustum.front = App->cameras->main_camera->owner->transform.GetGlobalFrontVector();
	sub_perspective_frustum.up = App->cameras->main_camera->owner->transform.GetGlobalUpVector();
	sub_perspective_frustum.verticalFov = App->cameras->main_camera->camera_frustum.verticalFov;
	sub_perspective_frustum.horizontalFov = App->cameras->main_camera->camera_frustum.horizontalFov;

	float game_camera_near_distance = App->cameras->main_camera->GetNearDistance();
	float game_camera_far_distance = App->cameras->main_camera->GetFarDistance();

	sub_perspective_frustum.nearPlaneDistance = game_camera_near_distance;
	switch (frustum_sub_division)
	{
	case LightFrustum::FrustumSubDivision::NEAR_FRUSTUM:
		sub_perspective_frustum.farPlaneDistance = game_camera_near_distance + (game_camera_far_distance - game_camera_near_distance) / 3.f;
		break;
	case LightFrustum::FrustumSubDivision::MID_FRUSTUM:
		sub_perspective_frustum.farPlaneDistance = game_camera_far_distance - (game_camera_far_distance - game_camera_near_distance) / 3.f;
		break;
	case LightFrustum::FrustumSubDivision::FAR_FRUSTUM:
		sub_perspective_frustum.farPlaneDistance = game_camera_far_distance;
		break;
	default:
		break;
	}
}

void LightFrustum::UpdateSubPerspectiveFrustumAABB()
{
	float3 sub_perspective_frustum_corner_points[8];
	sub_perspective_frustum.GetCornerPoints(sub_perspective_frustum_corner_points);
	sub_perspective_frustum_enclosing_aabb.SetNegativeInfinity();
	for (size_t i = 0; i < 8; ++i)
	{
		sub_perspective_frustum_enclosing_aabb.Enclose(App->lights->directional_light_rotation.Inverted() * sub_perspective_frustum_corner_points[i]);
	}
}

void LightFrustum::UpdateMeshRenderersAABB()
{
	mesh_renderers_enclosing_aabb.SetNegativeInfinity();
	std::vector<ComponentMeshRenderer*> culled_meshes = App->space_partitioning->GetCullingMeshes(sub_perspective_frustum, App->renderer->mesh_renderers);
	for (auto& mesh_renderer : culled_meshes)
	{
		AABB mesh_renderer_aabb = mesh_renderer->owner->aabb.bounding_box;
		mesh_renderer_aabb.TransformAsAABB(App->lights->directional_light_rotation.Inverted());
		mesh_renderers_enclosing_aabb.Enclose(mesh_renderer_aabb);
	}
}

void LightFrustum::UpdateLightOrthogonalFrustum()
{
	float3 min_point = sub_perspective_frustum_enclosing_aabb.minPoint;
	float3 max_point = sub_perspective_frustum_enclosing_aabb.maxPoint;

	light_orthogonal_frustum.type = FrustumType::OrthographicFrustum;
	light_orthogonal_frustum.pos = float3((max_point.x + min_point.x)*0.5f, (max_point.y + min_point.y)*0.5f, min_point.z);
	light_orthogonal_frustum.nearPlaneDistance = 0;
	light_orthogonal_frustum.farPlaneDistance = max_point.z - min_point.z;
	light_orthogonal_frustum.orthographicWidth = max_point.x - min_point.x;
	light_orthogonal_frustum.orthographicHeight = max_point.y - min_point.y;
}

void LightFrustum::RenderSubFrustum() const
{
	App->debug_draw->RenderPerspectiveFrustum(sub_perspective_frustum.ViewProjMatrix().Inverted(), float3::unitX);
}

void LightFrustum::RenderSubFrustumAABB() const
{
	float3 sub_perspective_frustum_aabb_corner_points[8];
	sub_perspective_frustum_enclosing_aabb.GetCornerPoints(sub_perspective_frustum_aabb_corner_points);
	for (size_t i = 0; i < 8; ++i)
	{
		sub_perspective_frustum_aabb_corner_points[i] = App->lights->directional_light_rotation * sub_perspective_frustum_aabb_corner_points[i];
	}
	App->debug_draw->RenderBox(sub_perspective_frustum_aabb_corner_points, float3::unitX);
}

void LightFrustum::RenderMeshRenderersAABB() const
{
	float3 mesh_renderers_enclosing_aabb_corner_points[8];
	mesh_renderers_enclosing_aabb.GetCornerPoints(mesh_renderers_enclosing_aabb_corner_points);
	for (size_t i = 0; i < 8; ++i)
	{
		mesh_renderers_enclosing_aabb_corner_points[i] = App->lights->directional_light_rotation * mesh_renderers_enclosing_aabb_corner_points[i];
	}
	App->debug_draw->RenderBox(mesh_renderers_enclosing_aabb_corner_points, float3::unitX);
}

