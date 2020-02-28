#ifndef _PREFABLOADER_H_
#define _PREFABLOADER_H_

#include <memory>
#include <string>

class Prefab;
class PrefabLoader
{
public:
	PrefabLoader() = default;
	~PrefabLoader() = default;
	static std::shared_ptr<Prefab> Load(const std::string& file_path);
};


#endif // !_PREFABLOADER_H_

