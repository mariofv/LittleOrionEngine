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
			copy_in_scene->is_prefab_parent = true;
			instances.push_back(copy_in_scene);
		}
		copy_in_scene->prefab_reference = this;
		copy_in_scene->transform.Translate(float3::zero); //:D
	}
}

void Prefab::Apply(GameObject * new_reference)
{
	auto result = App->resources->Import(exported_file, new_reference);
	if (result.first)
	{
		RecursiveRewrite(prefab.front().get(), new_reference, true, false);
		for (auto old_instance : instances)
		{
			if (new_reference == old_instance)
			{
				continue;
			}
			*old_instance << *new_reference;
			RecursiveRewrite(old_instance, new_reference, false, false);
		}
	}
}

void Prefab::RecursiveRewrite(GameObject * old_instance, GameObject * new_reference, bool original, bool revert)
{
	for (auto & child : new_reference->children)
	{
		auto it = std::find_if(old_instance->children.begin(), old_instance->children.end(), [child](auto old_instance_child) {
			return child->original_UUID == old_instance_child->original_UUID &&  child->original_UUID != 0;
		});
		//TODO: Only copy went their a different, need to implement == operator in every component ¿?¿
		if (it != old_instance->children.end())
		{
			**it << *child;
			RecursiveRewrite(*it, child, original, revert);
		}
		else
		{
			AddNewGameObjectToInstance(old_instance, child, original);
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
		if (original)
		{
			RemoveGameObjectFromOriginalPrefab(gameobject);
		}
		else
		{
			App->scene->RemoveGameObject(gameobject);
		}
	}

}

void Prefab::AddNewGameObjectToInstance(GameObject * parent, GameObject * new_reference, bool original)
{
	new_reference->original_UUID = new_reference->UUID;
	GameObject * copy;
	if (original)
	{
		prefab.emplace_back(std::make_unique<GameObject>());
		copy = prefab.back().get();
		copy->original_prefab = true;
	}
	else
	{
		copy = App->scene->CreateGameObject();
	}
	copy->SetParent(parent);
	*copy << *new_reference;
	copy->transform.SetTranslation(new_reference->transform.GetTranslation());
	for (auto new_reference_child : new_reference->children)
	{
		AddNewGameObjectToInstance(copy, new_reference_child, original);
	}
}

void Prefab::RemoveGameObjectFromOriginalPrefab(GameObject * gameobject_to_remove)
{
	auto it = std::remove_if(prefab.begin(), prefab.end(), [gameobject_to_remove](auto & gameobject) {
		return gameobject_to_remove->original_UUID == gameobject->original_UUID;
	});
	if (it != prefab.end())
	{
		prefab.erase(it);
		for (auto child : gameobject_to_remove->children)
		{
			RemoveGameObjectFromOriginalPrefab(child);
		}
	}
}