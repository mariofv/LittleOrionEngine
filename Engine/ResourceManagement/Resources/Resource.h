#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <pcg_basic.h>

class Config;
class Resource
{

public:
	Resource(const char * const UID) : UID(UID) {};
	virtual ~Resource() = default;
		
	virtual void Save(Config& config) const = 0;
	virtual void Load(const Config& config) = 0;

private:
	virtual void LoadInMemory() = 0;

private:
	const char * const UID;
};
#endif // !_RESOURCE_H_

