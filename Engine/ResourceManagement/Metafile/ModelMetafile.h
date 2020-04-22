#ifndef _MODELMETAFILE_H_
#define _MODELMETAFILE_H_
#include "Metafile.h"

class ModelMetafile : public Metafile
{

public:
	ModelMetafile() = default;
	~ModelMetafile() = default;

	void Save(Config& config) const override;
	void Load(const Config& config) override;


	//Model
	float scale = 1.0f;
	bool convert_units;
	std::string file_units;

	bool import_mesh = true;
	bool import_rig = true;
	bool import_animation = true;
	bool import_materials = true;


};
#endif // _MODELMETAFILE_H_
