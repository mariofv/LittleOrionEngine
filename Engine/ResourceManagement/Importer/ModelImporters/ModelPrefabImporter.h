#ifndef _MODELPREFABIMPORTER_H_
#define _MODELPREFABIMPORTER_H_
#include "ResourceManagement/Importer/Importer.h"

class Config;
class GameObject;
class ComponentMeshRenderer;
class ModelPrefabImporter : public Importer
{

public:
	ModelPrefabImporter() = default;
	~ModelPrefabImporter() = default;
	void ImportModelPrefab(const Config& model, const std::string& imported_file, std::string& exported_file) const;


private:
	void LoadNode(std::unique_ptr<GameObject>& parent_node, const Config& node_config, std::vector<std::string>& already_loaded_skeleton) const;
	std::unique_ptr<ComponentMeshRenderer> LoadMeshComponent(const std::string& mesh_exported_file, const std::string& material_exported_file, GameObject * node_game_object) const;
	void LoadSkeleton(const Config& node_config, std::vector<std::string>& already_loaded_skeleton, std::unique_ptr<GameObject>& parent_node) const;

};

#endif // !_MODELPREFABIMPORTER_H_

