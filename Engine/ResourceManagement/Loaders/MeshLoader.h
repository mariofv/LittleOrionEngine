#ifndef _MESHLOADER_H
#define _MESHLOADER_H
#include <memory>
#include <string>
class Mesh;
class MeshLoader
{
public:
	MeshLoader() = default;
	~MeshLoader() = default;
	static std::shared_ptr<Mesh> Load(const std::string& file_path);
};


#endif // !_MESHLOADER_H



