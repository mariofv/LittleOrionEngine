#ifndef _PREFABIMPORTER_H_
#define _PREFABIMPORTER_H_

#include "Importer.h"

class ComponentMeshRenderer;
class GameObject;
class Config;

class PrefabImporter : public Importer
{
public:
	PrefabImporter() : Importer(ResourceType::PREFAB) {};
	~PrefabImporter() = default;

	FileData ExtractData(Path& file_path) const override;
	FileData ExtractFromGameObject(GameObject* gameobject) const;
	FileData ExtractFromModel(const Config& model_config) const;

private:
	static void ExcractGameObjectFromNode(
		std::unique_ptr<GameObject>& parent_node, 
		const Config& node_config, 
		std::vector<std::unique_ptr<GameObject>>& game_objects,
		std::vector<std::unique_ptr<ComponentMeshRenderer>>& mesh_renderer_components,
		std::vector<uint32_t>& loaded_skeletons
	);
	static void ExcractMeshComponent(
		uint32_t mesh_uuid, uint32_t material_uuid, 
		std::vector<std::unique_ptr<ComponentMeshRenderer>>& mesh_renderer_components,
		GameObject* node_game_object
	);
	static void ExtractSkeletonFromNode(
		const Config& node_config,
		std::unique_ptr<GameObject>& parent_node,
		std::vector<std::unique_ptr<GameObject>>& game_objects,
		std::vector<uint32_t>& loaded_skeletons
	);
};

#endif