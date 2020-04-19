#ifndef _MODELPREFABIMPORTER_H_
#define _MODELPREFABIMPORTER_H_
#include "Component/ComponentMeshRenderer.h"
#include "ResourceManagement/Importer/Importer.h"
#include <vector>
class Config;
class GameObject;
class File;
class ModelPrefabImporter : public Importer
{

public:
	ModelPrefabImporter() = default;
	~ModelPrefabImporter() = default;
	void ImportModelPrefab(const Config& model, const File& imported_file) const;


private:
	void LoadNode(std::unique_ptr<GameObject>& parent_node, const Config& node_config, std::vector<std::string>& already_loaded_skeleton) const;
	void LoadMeshComponent(const std::string& mesh_exported_file, const std::string& material_exported_file, GameObject * node_game_object) const;
	std::string LoadSkeleton(const Config& node_config, std::vector<std::string>& already_loaded_skeleton, std::unique_ptr<GameObject>& parent_node) const;

};

#endif // !_MODELPREFABIMPORTER_H_

