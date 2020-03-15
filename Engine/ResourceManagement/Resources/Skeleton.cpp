#include "Skeleton.h"


Skeleton::Skeleton(const uint32_t UID, const std::string & exported_file) : Resource(UID, exported_file)
{

}

Skeleton::Skeleton(std::vector<Joint> && joints, const std::string & exported_file) :
skeleton(joints),
Resource(0, exported_file)
{
	LoadInMemory();

}

void Skeleton::LoadInMemory() {

}