#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "ResourceManagement/ImportOptions/ImportOptions.h"

#include <memory>
#include <string>
class Resource
{

public:
	Resource(uint32_t UID, const std::string & exported_file) : UID(UID), exported_file(exported_file) {
		int x = 0;
	};
	virtual ~Resource() = default;

public:
	std::string exported_file;
	std::string original_file;
private:
	virtual void LoadInMemory() = 0;

private:
	const uint32_t UID;

};

namespace Loader
{
	template<typename T>
	static std::shared_ptr<T> Load(const std::string& uid) {
		return nullptr;
	};
}
#endif // !_RESOURCE_H_

