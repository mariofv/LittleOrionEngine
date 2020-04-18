#ifndef _MATERIALMANAGER_H_
#define _MATERIALMANAGER_H_

#include <memory>
#include <string>

#include "Filesystem/File.h"

class Material;
class Metafile;
class Path;

class MaterialManager
{
public:
	MaterialManager() = default;
	~MaterialManager() = default;

	static FileData Binarize(const Material& material);
	static std::shared_ptr<Material> Load(Metafile* metafile, const FileData& resource_data);
	static FileData Create();
};

#endif // !_MATERIALMANAGER_H_



