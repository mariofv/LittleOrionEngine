#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <pcg_basic.h>
class Resource
{

public:
	Resource(const char * const UID) : UID(UID) {};
	virtual ~Resource() = default;
		
private:
	const char * const UID;
};
#endif // !_RESOURCE_H_

