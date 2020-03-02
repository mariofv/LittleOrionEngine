#include "Prefab.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleResourceManager.h"


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
		copy_in_scene->transform.Translate(float3::zero); //:D
	}
}

void Prefab::Rewrite(GameObject * new_reference)
{
	auto result = App->resources->Import(exported_file, new_reference);
	if (result.first)
	{
		for (auto old_instance : instances)
		{
			if (new_reference == old_instance)
			{
				continue;
			}
			*old_instance << *new_reference;
			for (auto & child : new_reference->children)
			{
				auto it = std::find_if(old_instance->children.begin(), old_instance->children.end(), [child](auto old_instance_child) {
					return child->original_UUID == old_instance_child->original_UUID;
				});
				//TODO: Only copy went their a different, need to implemente == operator in every component ¿?¿
				if (it != old_instance->children.end())
				{
					**it << *child;
				}
				else
				{
					child->original_UUID = child->UUID;
					GameObject * copy = App->scene->CreateGameObject();
					copy = child;
					copy->SetParent(old_instance);
				}
			}

		}
	}
}

void Prefab::RecursiveRewrite(GameObject * new_reference)
{

}

