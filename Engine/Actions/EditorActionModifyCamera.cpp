#include "EditorActionModifyCamera.h"



EditorActionModifyCamera::EditorActionModifyCamera(ComponentCamera* comp)
{
	component = comp;
	mov_speed = component->camera_movement_speed;
	fov = component->camera_frustum.verticalFov;
	aspect_ratio = component->aspect_ratio;
	near_plane = component->camera_frustum.nearPlaneDistance;
	far_plane = component->camera_frustum.farPlaneDistance;
	ortographic_size = component->camera_frustum.orthographicHeight;
	depth = component->depth;
}


EditorActionModifyCamera::~EditorActionModifyCamera()
{
}

void EditorActionModifyCamera::Undo()
{
	float mov_speed_aux = component->camera_movement_speed;
	float fov_aux = component->camera_frustum.verticalFov;
	float aspect_ratio_aux = component->aspect_ratio;
	float near_plane_aux = component->camera_frustum.nearPlaneDistance;
	float far_plane_aux = component->camera_frustum.farPlaneDistance;
	float ortographic_size_aux = component->camera_frustum.orthographicHeight;
	float depth_aux = component->depth;

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
