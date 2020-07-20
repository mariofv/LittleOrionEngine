#ifndef _MESHMANAGER_H_
#define _MESHMANAGER_H_

#include "Filesystem/File.h"
#include "Helper/Timer.h"

#include <memory>
#include <string>


class Metafile;
class Mesh;
class Path;

class MeshManager
{
public:
	MeshManager() = default;
	~MeshManager() = default;

	static std::shared_ptr<Mesh> Load(uint32_t uuid, const FileData& resource_data, bool async = false);

	static Timer timer;
};

#endif // !_MESHMANAGER_H_



