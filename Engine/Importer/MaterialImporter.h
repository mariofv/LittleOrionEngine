#ifndef _MATERIALIMPORTER_H_
#define _MATERIALIMPORTER_H_

#include "Importer/Importer.h"

#include <vector>
#include <string>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

class Texture;
struct aiScene;

class MaterialImporter : Importer
{
public:
	MaterialImporter();
	bool Import(const char* file_path, std::string& output_file) override;
	void ImportMaterialFromMesh(const aiScene* scene, size_t mesh_index, const char* file_path,std::vector<std::string> & loaded_meshes_materials);
	std::string ImportMaterialData(const std::string & material_path, const std::string model_base_path);
	std::shared_ptr<Texture> Load(const char* file_path);
	unsigned int LoadCubemap(std::vector<std::string> faces_paths) const;

	void RemoveTextureFromCacheIfNeeded(std::shared_ptr<Texture> texture);

private:
	ILubyte* LoadImageData(const char* file_path, int & width, int & height, int image_type) const;
	std::string GetTextureFileName(const char *texture_file_path) const;
	std::shared_ptr<ModuleFileSystem::File> GetAlreadyImportedMaterial(const ModuleFileSystem::File & file) const;
private:
	const std::string LIBRARY_TEXTURES_FOLDER = ".//Library//Materials";
	std::vector<std::shared_ptr<Texture>> texture_cache;
};
#endif // !_MATERIALIMPORTER_H_
