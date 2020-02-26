#include <ResourceManagement/Resources/Prefab.h>
#include <Main/Application.h>
#include <Module/ModuleScene.h>

void Prefab::Instantiate(const GameObject * prefab_parent)
{
	for (auto & gameObject : prefab)
	{
		GameObject copy = *gameObject.get();
		App->scene->AddGameObject(copy);
	}
}