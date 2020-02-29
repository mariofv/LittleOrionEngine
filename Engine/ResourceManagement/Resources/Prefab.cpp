#include <ResourceManagement/Resources/Prefab.h>
#include <Main/Application.h>
#include <Module/ModuleScene.h>
#include <map>

Prefab::Prefab(std::vector<std::unique_ptr<GameObject>> && gameObjects, uint32_t UID, const std::string & exported_file) : Resource(UID, exported_file), prefab(std::move(gameObjects))
{

}
void Prefab::Instantiate(GameObject * prefab_parent)
{
	std::map<uint64_t, GameObject*> original_gameObject_reference;

	for (auto & gameObject : prefab)
	{
		GameObject* copy_in_scene = App->scene->AddGameObject(std::make_unique<GameObject>(*gameObject.get()));
		original_gameObject_reference[gameObject->UUID] = copy_in_scene;

		if (gameObject->parent != nullptr && original_gameObject_reference.find(gameObject->parent->UUID) != original_gameObject_reference.end())
		{
			copy_in_scene->SetParent(original_gameObject_reference[gameObject->parent->UUID]);
		}
		else
		{
			copy_in_scene->prefab_reference = this;
			instances.push_back(copy_in_scene);
		}
	}
}
void Prefab::LoadInMemory()
{

}