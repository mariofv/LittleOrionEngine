#ifndef _MESHIMPORTER_H_
#define _MESHIMPORTER_H_
#include "Importer.h"

class MeshImporter : Importer
{
public:
	MeshImporter() = default;
	bool Import(const char* file_path, std::string& output_file) const override { return true; };
};
#endif // !_MESHIMPORTER_H_
