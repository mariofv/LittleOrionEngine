#ifndef _MODULEINPUT_H_
#define _MODULEINPUT_H_

#include "Module.h"
#include "Globals.h"

typedef unsigned __int8 Uint8;
struct SDL_Cursor;

class ModuleInput : public Module
{
public:
	ModuleInput() = default;
	~ModuleInput() = default;

	bool Init();
	update_status PreUpdate();
	bool CleanUp();

	void ShowInputOptions();

private:
	enum class FileType
	{
		MODEL,
		TEXTURE,
		UNKNOWN
	};
	
	FileType GetFileType(const char *file_path);
	std::string GetFileExtension(const char *file_path);

private:
	const Uint8 *keyboard = nullptr;
};

#endif //_MODULEINPUT_H_