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
	std::vector<Config> nodes_config;
	nodes_config.reserve(nodes.size());
	for (const auto & node : nodes)
	{
		assert(node->resource_type != ResourceType::UNKNOWN);
		Config node_config;
		node->Save(node_config);
		nodes_config.emplace_back(node_config);
	}
	assert(nodes_config.size() == nodes.size());
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
		std::unique_ptr<Metafile> node = std::make_unique<Metafile>();
		node->Load(node_config);
		assert(node->resource_type != ResourceType::UNKNOWN);
		nodes.push_back(std::move(node));
	}

}

void ModelMetafile::GetModelNode(Metafile& model_node_metafile) const
{
	const auto it = std::find_if(nodes.begin(), nodes.end(), [&model_node_metafile](const auto & node) {
		return node->resource_name == model_node_metafile.resource_name && model_node_metafile.resource_type == node->resource_type;
	});
	if (it != nodes.end())
	{
		model_node_metafile = *((*it).get());
	}
}
