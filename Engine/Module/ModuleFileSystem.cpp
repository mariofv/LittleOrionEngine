#include "ModuleFileSystem.h"


unsigned int ModuleFileSystem::Load(const char* path, const char* file, char** buffer) const
{
	return 0;
}
unsigned int ModuleFileSystem::Save(const char* file, const void* buffer, unsigned int size, bool append) const
{
	return 0;
}
bool ModuleFileSystem::Remove(const char* file)
{
	return true;
}
bool ModuleFileSystem::Exists(const char* file) const
{
	return true;
}
bool ModuleFileSystem::MakeDirectory(const char* directory)
{
	return true;
}
bool ModuleFileSystem::IsDirectory(const char* file) const 
{
	return true;
}
bool ModuleFileSystem::Copy(const char* source, const char* destination)
{
	return true;
}
