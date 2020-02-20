#include "ModuleDebug.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleModelLoader.h"
#include "Module/ModuleScene.h"

#include <random>
#include <ctime>
#include <GL/glew.h>

// Called before render is available
bool ModuleDebug::Init()
{
	APP_LOG_SECTION("************ Module Debug Init ************");

	
	APP_LOG_SUCCESS("Module Debug initialized correctly.")

	return true;
}

// Called before quitting
bool ModuleDebug::CleanUp()
{
	APP_LOG_INFO("Destroying Debug");	
	return true;
}

void ModuleDebug::CreateHousesRandom() const
{
	std::srand(static_cast<unsigned int>(std::time(nullptr))); // use current time as seed for random generator
	GameObject *houses = App->scene->CreateGameObject();
	for (int i = 0; i < num_houses; ++i)
	{
		GameObject *loaded_house = App->model_loader->LoadModel(HOUSE_MODEL_PATH);
		float x = static_cast<float>(std::rand() % max_dispersion_x);
		float z = static_cast<float>(std::rand() % max_dispersion_z);
		loaded_house->transform.SetTranslation(float3(x, 0, z));
		houses->AddChild(loaded_house);
	}
	houses->SetStatic(true);
}