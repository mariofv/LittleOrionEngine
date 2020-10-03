#include "LightFrustum.h"

#include "Component/ComponentMeshRenderer.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleDebugDraw.h"
#include "Module/ModuleLight.h"
#include "Module/ModuleSpacePartitioning.h"
#include "Module/ModuleRender.h"

LightFrustum::LightFrustum(FrustumSubDivision frustum_sub_division) : frustum_sub_division(frustum_sub_division)
{
	switch (frustum_sub_division)
	{
	case LightFrustum::FrustumSubDivision::NEAR_FRUSTUM:
		sub_perspective_frustum_render_color = float3::unitX;
		sub_perspective_frustum_enclosing_aabb_render_color = float3(1.f, 0.2f, 0.f);
		mesh_renderers_enclosing_aabb_render_color = float3(1.f, 0.f, 0.4f);
		light_orthogonal_frustum_render_color = float3(1.f, 0.6f, 0.f);
		multiplier = 5.f;
		break;

	case LightFrustum::FrustumSubDivision::MID_FRUSTUM:
		sub_perspective_frustum_render_color = float3::unitY;
		sub_perspective_frustum_enclosing_aabb_render_color = float3(0.2f, 1.f, 0.f);
		mesh_renderers_enclosing_aabb_render_color = float3(0.f, 1.f, 0.4f);
		light_orthogonal_frustum_render_color = float3(0.6f, 1.f, 0.f);
		multiplier = 3.f;
		break;

	case LightFrustum::FrustumSubDivision::FAR_FRUSTUM:
		sub_perspective_frustum_render_color = float3::unitZ;
		sub_perspective_frustum_enclosing_aabb_render_color = float3(0.2f, 0.f, 1.f);
		mesh_renderers_enclosing_aabb_render_color = float3(0.f, 0.4f, 1.f);
		light_orthogonal_frustum_render_color = float3(0.6f, 0.f, 1.f);
		multiplier = 1.f;
		break;

	case LightFrustum::FrustumSubDivision::FULL_FRUSTUM:
		sub_perspective_frustum_render_color = float3(0.5f);
		sub_perspective_frustum_enclosing_aabb_render_color = float3(0.6f);
		mesh_renderers_enclosing_aabb_render_color = float3(0.7f);
		light_orthogonal_frustum_render_color = float3(0.8f);
		multiplier = 1.f;
		break;

	default:
		break;
	}
}

void LightFrustum::Update()
{
	if (App->cameras->main_camera == nullptr)
	{
		return;
	}

	UpdateSubPerspectiveFrustum();
	if (frustum_sub_division != FrustumSubDivision::FULL_FRUSTUM)
	{
		UpdateSubPerspectiveFrustumAABB();
	}
	UpdateMeshRenderersAABB();
	UpdateLightOrthogonalFrustum();
}

void LightFrustum::UpdateSubPerspectiveFrustum()
{
	if (frustum_sub_division == LightFrustum::FrustumSubDivision::FULL_FRUSTUM)
	{
		sub_perspective_frustum = App->cameras->main_camera->camera_frustum;
		return;
	}

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
	std::vector<ComponentMeshRenderer*> culled_meshes = App->space_partitioning->GetCullingMeshes(
		sub_perspective_frustum,
		App->renderer->mesh_renderers, 
		ComponentMeshRenderer::MeshProperties::SHADOW_CASTER
	);
	for (auto& mesh_renderer : culled_meshes)
	{
		AABB mesh_renderer_aabb = mesh_renderer->owner->aabb.bounding_box;
		OBB mesh_renderer_obb = mesh_renderer_aabb.Transform(App->lights->directional_light_rotation.Inverted());
		AABB mesh_renderer_light_space_aabb = mesh_renderer_obb.MinimalEnclosingAABB();

		mesh_renderers_enclosing_aabb.Enclose(mesh_renderer_light_space_aabb);
	}
}

void LightFrustum::UpdateLightOrthogonalFrustum()
{
	AABB intersected_aabb;
	if (frustum_sub_division == FrustumSubDivision::FULL_FRUSTUM)
	{
		intersected_aabb = mesh_renderers_enclosing_aabb;
	}
	else
	{
		intersected_aabb = sub_perspective_frustum_enclosing_aabb.Intersection(mesh_renderers_enclosing_aabb);
	}
	float3 min_point = intersected_aabb.minPoint;
	float3 max_point = intersected_aabb.maxPoint;

	light_orthogonal_frustum.type = FrustumType::OrthographicFrustum;
	light_orthogonal_frustum.pos = App->lights->directional_light_rotation * float3((max_point.x + min_point.x)*0.5f, (max_point.y + min_point.y)*0.5f, min_point.z);
	light_orthogonal_frustum.nearPlaneDistance = 0;
	light_orthogonal_frustum.front = App->lights->directional_light_rotation * float3::unitZ;
	light_orthogonal_frustum.up = App->lights->directional_light_rotation * float3::unitY;
	light_orthogonal_frustum.farPlaneDistance = max_point.z - min_point.z + LIGHT_FRUSTUM_FAR_PLANE_EXTRA_DISTANCE;
	light_orthogonal_frustum.orthographicWidth = max_point.x - min_point.x;
	light_orthogonal_frustum.orthographicHeight = max_point.y - min_point.y;
}

void LightFrustum::RenderSubFrustum() const
{
	App->debug_draw->RenderPerspectiveFrustum(sub_perspective_frustum.ViewProjMatrix().Inverted(), sub_perspective_frustum_render_color);
}

void LightFrustum::RenderSubFrustumAABB() const
{
	float3 sub_perspective_frustum_aabb_corner_points[8];
	sub_perspective_frustum_enclosing_aabb.GetCornerPoints(sub_perspective_frustum_aabb_corner_points);
	for (size_t i = 0; i < 8; ++i)
	{
		sub_perspective_frustum_aabb_corner_points[i] = App->lights->directional_light_rotation * sub_perspective_frustum_aabb_corner_points[i];
	}
	App->debug_draw->RenderBox(sub_perspective_frustum_aabb_corner_points, sub_perspective_frustum_enclosing_aabb_render_color);
}

void LightFrustum::RenderMeshRenderersAABB() const
{
	float3 mesh_renderers_enclosing_aabb_corner_points[8];
	mesh_renderers_enclosing_aabb.GetCornerPoints(mesh_renderers_enclosing_aabb_corner_points);
	for (size_t i = 0; i < 8; ++i)
	{
		mesh_renderers_enclosing_aabb_corner_points[i] = App->lights->directional_light_rotation * mesh_renderers_enclosing_aabb_corner_points[i];
	}
	App->debug_draw->RenderBox(mesh_renderers_enclosing_aabb_corner_points, mesh_renderers_enclosing_aabb_render_color);
}

void LightFrustum::RenderLightFrustum() const
{
	float3 light_orthogonal_frustum_corner_points[8];
	light_orthogonal_frustum.GetCornerPoints(light_orthogonal_frustum_corner_points);
	for (size_t i = 0; i < 8; ++i)
	{
		light_orthogonal_frustum_corner_points[i] = light_orthogonal_frustum_corner_points[i];
	}

	std::swap(light_orthogonal_frustum_corner_points[2], light_orthogonal_frustum_corner_points[3]);
	std::swap(light_orthogonal_frustum_corner_points[6], light_orthogonal_frustum_corner_points[7]);

	App->debug_draw->RenderBox(light_orthogonal_frustum_corner_points, light_orthogonal_frustum_render_color);
	float4x4 light_frustum_transform = App->lights->directional_light_rotation.ToFloat4x4() * float4x4::Translate(light_orthogonal_frustum.pos);
	App->debug_draw->RenderAxis(light_frustum_transform, 1.f, 1.f, light_orthogonal_frustum_render_color);
}

