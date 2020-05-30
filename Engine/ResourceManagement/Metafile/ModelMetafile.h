#ifndef _MODELMETAFILE_H_
#define _MODELMETAFILE_H_
#include "Metafile.h"
#include <vector>
class ModelMetafile : public Metafile
{

public:

	ModelMetafile() = default;
	~ModelMetafile() = default;

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	void GetModelNode(Metafile& model_node_metafile ) const;
	//Model
	float scale_factor = 0.01f;
	bool convert_units = true;

	bool import_mesh = true;
	bool import_rig = true;
	bool import_animation = true;
	bool import_material = true;

	//Skeleton
	bool complex_skeleton = false;

	std::vector<std::unique_ptr<Metafile>> nodes;


};
#endif // _MODELMETAFILE_H_
