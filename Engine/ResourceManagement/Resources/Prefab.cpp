#include <ResourceManagement/Resources/Prefab.h>
#include <Main/Application.h>
#include <Module/ModuleScene.h>


Prefab::Prefab(uint32_t UID, const std::string & exported_file) : Resource(UID, exported_file)
{

}
void Prefab::Instantiate(const GameObject * prefab_parent)
{
	for (auto & gameObject : prefab)
	{
		GameObject copy = *gameObject.get();
		App->scene->AddGameObject(copy);
	}
}

void Prefab::LoadInMemory()
{

}