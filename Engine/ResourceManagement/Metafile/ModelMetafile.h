#ifndef _MODELMETAFILE_H_
#define _MODELMETAFILE_H_
#include "Metafile.h"
#include <vector>
class ModelMetafile : public Metafile
{

public:

	struct ModelNode
	{
		ResourceType type = ResourceType::UNKNOWN;
		std::string unique_name;
		uint32_t uuid = 0;
	};
	ModelMetafile() = default;
	~ModelMetafile() = default;

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	void GetModelNode(ModelNode& model_node ) const;
	//Model
	float scale_factor = 0.01f;
	bool convert_units = true;

	bool import_mesh = true;
	bool import_rig = true;
	bool import_animation = true;
	bool import_material = true;

	//Skeleton
	bool complex_skeleton = false;

	std::vector<ModelNode> nodes;


};
#endif // _MODELMETAFILE_H_
