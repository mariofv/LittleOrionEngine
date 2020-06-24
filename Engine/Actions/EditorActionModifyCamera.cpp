#include "EditorActionModifyCamera.h"
#include "Module/ModuleScene.h"
#include "Main/Application.h"


EditorActionModifyCamera::EditorActionModifyCamera(ComponentCamera* comp) : component_UUID(comp->UUID), mov_speed(comp->camera_movement_speed),
fov(comp->camera_frustum.verticalFov), aspect_ratio(comp->aspect_ratio), near_plane(comp->camera_frustum.nearPlaneDistance),
far_plane(comp->camera_frustum.farPlaneDistance), ortographic_size(comp->camera_frustum.orthographicHeight), depth(comp->depth){}

void EditorActionModifyCamera::Undo()
{
	ComponentCamera* component = (ComponentCamera*)App->scene->GetComponent(component_UUID);

	float mov_speed_aux = component->camera_movement_speed;
	float fov_aux = component->camera_frustum.verticalFov;
	float aspect_ratio_aux = component->aspect_ratio;
	float near_plane_aux = component->camera_frustum.nearPlaneDistance;
	float far_plane_aux = component->camera_frustum.farPlaneDistance;
	float ortographic_size_aux = component->camera_frustum.orthographicHeight;
	int depth_aux = component->depth;

	component->camera_movement_speed = mov_speed;
	component->SetFOV(fov);
	component->SetAspectRatio(aspect_ratio);
	component->SetNearDistance(near_plane);
	component->SetFarDistance(far_plane);
	component->depth = depth;

	mov_speed = mov_speed_aux;
	fov = fov_aux;
	aspect_ratio = aspect_ratio_aux;
	near_plane = near_plane_aux;
	far_plane = far_plane_aux;
	ortographic_size = ortographic_size_aux;
	depth = depth_aux;
}

void EditorActionModifyCamera::Redo()
{
	Undo();
}
