#ifndef _MESHIMPORTER_H_
#define _MESHIMPORTER_H_
#include "Importer.h"


class aiMesh;
class ComponentMesh;
class MeshImporter : Importer
{
public:
	MeshImporter();
	bool Import(const char* file_path, std::string& output_file) const override;
	void Load(const char* file_path, ComponentMesh & component_mesh) const;

private:
	void ImportMesh(const aiMesh* file_path, const std::string& output_file) const;
	const std::string LIBRARY_MESHES_FOLDER = ".//Library//Meshes";

};
#endif // !_MESHIMPORTER_H_
