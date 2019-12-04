#include "Skybox.h"
#include "Application.h"
#include "Module/ModuleTexture.h"

Skybox::Skybox() 
{
	std::string cube_face_front_path = "resources/skyboxes/sor_lake1/lake1_ft.jpg";
	std::string cube_face_back_path = "resources/skyboxes/sor_lake1/lake1_bk.jpg";

	std::string cube_face_left_path = "resources/skyboxes/sor_lake1/lake1_lf.jpg";
	std::string cube_face_right_path = "resources/skyboxes/sor_lake1/lake1_rt.jpg";

	std::string cube_face_up_path = "resources/skyboxes/sor_lake1/lake1_up.jpg";
	std::string cube_face_down_path = "resources/skyboxes/sor_lake1/lake1_dn.jpg";

	std::vector<std::string> faces{
		cube_face_front_path,
		cube_face_back_path,
		cube_face_left_path,
		cube_face_right_path,
		cube_face_up_path,
		cube_face_down_path
	};

	skybox_texture = App->texture->LoadCubemap(faces);
}

Skybox::~Skybox()
{
	glDeleteTextures(1, &skybox_texture);
}