#include "ModelMetafile.h"

#include "Helper/Config.h"
#include <algorithm>
void ModelMetafile::Save(Config& config) const
{
	Metafile::Save(config);
	config.AddFloat(scale_factor, "ScaleFactor");
	config.AddBool(convert_units, "Convert");

	config.AddBool(import_mesh, "ImportMesh");
	config.AddBool(import_rig, "ImportRig");
	config.AddBool(import_animation, "ImportAnimation");
	config.AddBool(import_material, "ImportMaterial");

	config.AddBool(complex_skeleton, "ComplexSkeleton");
	std::vector<Config> nodes_config (nodes.size());
	for (const auto & node : nodes)
	{
		Config node_config;
		node_config.AddUInt(node.uuid, "UUID");
		node_config.AddString(node.unique_name, "Name");
		node_config.AddInt(static_cast<int>(node.type), "Type");
		nodes_config.push_back(node_config);
	}

	config.AddChildrenConfig(nodes_config, "Nodes");

}

void ModelMetafile::Load(const Config& config)
{
	Metafile::Load(config);

	scale_factor = config.GetFloat("ScaleFactor", 0.01f);
	convert_units = config.GetBool("Convert", true);


	import_mesh = config.GetBool("ImportMesh", true);
	import_rig = config.GetBool("ImportRig",true);
	import_animation = config.GetBool("ImportAnimation", true);
	import_material = config.GetBool( "ImportMaterial", true);

	complex_skeleton = config.GetBool("ComplexSkeleton", false);

	std::vector<Config> nodes_config;
	config.GetChildrenConfig("Nodes", nodes_config);
	for (const auto & node_config : nodes_config)
	{
		ModelNode node;
		node.uuid = node_config.GetUInt("UUID", 0);
		node_config.GetString("Name", node.unique_name, {});
		node.type = static_cast<ResourceType>(node_config.GetInt("Type", static_cast<int>(ResourceType::UNKNOWN)));
		nodes.push_back(node);
	}

}

void ModelMetafile::GetModelNode(ModelNode& model_node) const
{
	const auto it = std::find_if(nodes.begin(), nodes.end(), [&model_node](const auto & node) {
		return node.unique_name == model_node.unique_name && model_node.type == node.type;
	});
	if (it != nodes.end())
	{
		model_node = *it;
	}
}
