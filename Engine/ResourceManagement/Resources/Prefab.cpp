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
			RecursiveRewrite(old_instance, new_reference);
		}
	}
}

void Prefab::RecursiveRewrite(GameObject * old_instance, GameObject * new_reference)
{
	for (auto & child : new_reference->children)
	{
		auto it = std::find_if(old_instance->children.begin(), old_instance->children.end(), [child](auto old_instance_child) {
			return child->original_UUID == old_instance_child->original_UUID;
		});
		//TODO: Only copy went their a different, need to implemente == operator in every component ¿?¿
		if (it != old_instance->children.end())
		{
			**it << *child;
			RecursiveRewrite(*it, child);
		}
		else if(child->original_UUID == 0)
		{
			AddNewGameObjectToInstance(old_instance, child);
		}
	}
	std::vector<GameObject*> gameobjects_to_remove;
	std::copy_if(
		old_instance->children.begin(),
		old_instance->children.end(),
		std::back_inserter(gameobjects_to_remove),
		[&new_reference](auto child)
	{
		return std::find_if(new_reference->children.begin(), new_reference->children.end(),
			[child](GameObject * reference_child)
		{
			return reference_child->original_UUID == child->original_UUID;
		}) == new_reference->children.end() && child->original_UUID != 0;
	}

	);
	for (auto gameobject : gameobjects_to_remove)
	{
		App->scene->RemoveGameObject(gameobject);
	}
}

void Prefab::AddNewGameObjectToInstance(GameObject * parent, GameObject * new_reference)
{
	new_reference->original_UUID = new_reference->UUID;
	GameObject * copy = App->scene->CreateGameObject();
	copy->SetParent(parent);
	*copy << *new_reference;
	copy->transform.SetTranslation(new_reference->transform.GetTranslation());
	for (auto new_reference_child : new_reference->children)
	{
		AddNewGameObjectToInstance(copy, new_reference_child);
	}
}