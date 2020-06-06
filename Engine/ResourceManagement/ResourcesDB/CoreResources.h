#ifndef _CORERESOURCES_H_
#define _CORERESOURCES_H_

#include <unordered_map>

enum class CoreResource
{
	UNKNOWN = 0,
	CUBE = 1,
	SPHERE = 2,
	TORUS = 3,
	CYLINDER = 4,
	QUAD = 5,

	DEFAULT_MATERIAL = 6,

	DEFAULT_SKYBOX = 7,
	SKYBOX_TEXTURE_RIGHT = 8,
	SKYBOX_TEXTURE_LEFT = 9,
	SKYBOX_TEXTURE_UP = 10,
	SKYBOX_TEXTURE_DOWN = 11,
	SKYBOX_TEXTURE_FRONT = 12,
	SKYBOX_TEXTURE_BACK = 13,

	BILLBOARD_CAMERA_TEXTURE = 14,
	BILLBOARD_LIGHT_TEXTURE = 15,
	BILLBOARD_DEFAULT_TEXTURE = 16,

	DEFAULT_FONT = 17,

	NUM_CORE_RESOURCES = 18
};

static const uint32_t NUM_CORE_RESOURCES = (uint32_t)CoreResource::NUM_CORE_RESOURCES;

static std::unordered_map<std::string, CoreResource> core_resources_uuid_mapping({
	{ "/Resources/Primitives/Cube/Cube_0.mesh", CoreResource::CUBE },
	{ "/Resources/Primitives/Sphere/Sphere_0.mesh", CoreResource::SPHERE },
	{ "/Resources/Primitives/Torus/Torus_0.mesh", CoreResource::TORUS },
	{ "/Resources/Primitives/Cylinder/Cylinder_0.mesh", CoreResource::CYLINDER },
	{ "/Resources/Primitives/Quad/Plane_0.mesh", CoreResource::QUAD },

	{ "/Resources/Materials/default.mat", CoreResource::DEFAULT_MATERIAL },

	{ "/Resources/Skyboxes/ame_nebula/default.skybox", CoreResource::DEFAULT_SKYBOX },
	{ "/Resources/Skyboxes/ame_nebula/purplenebula_rt.tga", CoreResource::SKYBOX_TEXTURE_RIGHT },
	{ "/Resources/Skyboxes/ame_nebula/purplenebula_lf.tga", CoreResource::SKYBOX_TEXTURE_LEFT },
	{ "/Resources/Skyboxes/ame_nebula/purplenebula_up.tga", CoreResource::SKYBOX_TEXTURE_UP },
	{ "/Resources/Skyboxes/ame_nebula/purplenebula_dn.tga", CoreResource::SKYBOX_TEXTURE_DOWN },
	{ "/Resources/Skyboxes/ame_nebula/purplenebula_ft.tga", CoreResource::SKYBOX_TEXTURE_FRONT },
	{ "/Resources/Skyboxes/ame_nebula/purplenebula_bk.tga", CoreResource::SKYBOX_TEXTURE_BACK },

	{ "/Resources/Textures/video-solid.png", CoreResource::BILLBOARD_CAMERA_TEXTURE },
	{ "/Resources/Textures/lightbulb-solid.png", CoreResource::BILLBOARD_LIGHT_TEXTURE },

	{ "/Resources/Textures/Leaves Tree Set 1 Life Difuse Alpha.tga", CoreResource::BILLBOARD_DEFAULT_TEXTURE},

	{ "/Resources/Fonts/Montserrat-Light.ttf", CoreResource::DEFAULT_FONT }
});

#endif // !_CORERESOURCES_H_
