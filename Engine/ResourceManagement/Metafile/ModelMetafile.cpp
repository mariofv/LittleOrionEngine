#include "ModelMetafile.h"

#include "Helper/Config.h"

void ModelMetafile::Save(Config& config) const
{
	Metafile::Save(config);
	config.AddFloat(scale_factor, "ScaleFactor");
	config.AddBool(convert_units, "Convert");

	config.AddBool(import_mesh, "ImportMesh");
	config.AddBool(import_rig, "ImportRig");
	config.AddBool(import_animation, "ImportAnimation");
	config.AddBool(import_material, "ImportMaterial");

}

void ModelMetafile::Load(const Config& config)
{
	Metafile::Load(config);

	scale_factor = config.GetFloat("ScaleFactor", 0.01f);
	convert_units = config.GetBool("Convert", false);


	import_mesh = config.GetBool("ImportMesh", true);
	import_rig = config.GetBool("ImportRig",true);
	import_animation = config.GetBool("ImportAnimation", true);
	import_material = config.GetBool( "ImportMaterial", true);
}
