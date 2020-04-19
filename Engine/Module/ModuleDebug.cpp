#include "ModuleDebug.h"

#include "Filesystem/Path.h"
#include "Helper/TemplatedGameObjectCreator.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleResourceManager.h"
#include "Module/ModuleScene.h"

#include "ResourceManagement/Importer/Importer.h"
#include "ResourceManagement/ResourcesDB/CoreResources.h"
#include "ResourceManagement/Resources/Prefab.h"

#include <ctime>
#include <GL/glew.h>
#include <random>

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

void ModuleDebug::CreateFrustumCullingDebugScene() const
{
	std::srand(static_cast<unsigned int>(std::time(nullptr))); // use current time as seed for random generator
	
	GameObject *cubes = App->scene->CreateGameObject();
	
	for (int i = 0; i < num_houses; ++i)
	{
		GameObject* loaded_cube = TemplatedGameObjectCreator::CreatePrimitive(CoreResource::CUBE);
		loaded_cube->SetParent(cubes);

		float x = static_cast<float>(std::rand() % max_dispersion_x);
		float z = static_cast<float>(std::rand() % max_dispersion_z);
		loaded_cube->transform.SetTranslation(float3(x, 0, z));
	}
	cubes->SetStatic(true);
}