#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <pcg_basic.h>
#include <memory>
#include <string>
class Config;
class Resource
{

public:
	Resource(const char * const UID, const std::string & exported_file) : UID(UID), exported_file(exported_file) {};
	virtual ~Resource() = default;
		
	virtual void Save(Config& config) const = 0;
	virtual void Load(const Config& config) = 0;

public:
	std::string exported_file;

private:
	virtual void LoadInMemory() = 0;

private:
	const char * const UID;

};

namespace Loader
{
	template<typename T>
	static std::shared_ptr<T> Load(const std::string& uid) {
		return nullptr;
	};
}
#endif // !_RESOURCE_H_

