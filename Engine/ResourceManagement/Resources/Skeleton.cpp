#include "Skeleton.h"


Skeleton::Skeleton(const char * const UID, const std::string & exported_file) : Resource(UID, exported_file)
{

}

Skeleton::Skeleton(std::vector<Joint> && joints, const std::string & exported_file) :
skeleton(joints),
Resource("", exported_file)
{
	LoadInMemory();

}
void Skeleton::Save(Config& config) const
{
}
void Skeleton::Load(const Config& config) {
}

void Skeleton::LoadInMemory() {

}