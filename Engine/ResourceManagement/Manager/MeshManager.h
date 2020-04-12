#ifndef _MESHMANAGER_H_
#define _MESHMANAGER_H_

#include <memory>
#include <string>

#include "Filesystem/File.h"

class Metafile;
class Mesh;
class Path;

class MeshManager
{
public:
	MeshManager() = default;
	~MeshManager() = default;

	static std::shared_ptr<Mesh> Load(Metafile* metafile, const FileData& resource_data);
};

#endif // !_MESHMANAGER_H_



