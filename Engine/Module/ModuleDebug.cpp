#include "ModuleDebug.h"
#include "Filesystem/Path.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleScene.h"
#include "ResourceManagement/Importer/Importer.h"
#include "ResourceManagement/Resources/Prefab.h"
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
	ImportOptions options;
	Importer::GetOptionsFromMeta(Importer::GetMetaFilePath(Path(HOUSE_MODEL_PATH)), options);
	auto prefab = App->resources->Load<Prefab>(options.exported_file);

	for (int i = 0; i < num_houses; ++i)
	{
		GameObject* loaded_house = prefab->Instantiate(houses);

		float x = static_cast<float>(std::rand() % max_dispersion_x);
		float z = static_cast<float>(std::rand() % max_dispersion_z);
		loaded_house->transform.SetTranslation(float3(x, 0, z));
	}
	houses->SetStatic(true);
}