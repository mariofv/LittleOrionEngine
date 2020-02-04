#ifndef _MODULE_H_
#define _MODULE_H_

#include "Main/Globals.h"

class Application;

class Module
{
public:

	Module() = default;

	virtual bool Init() 
	{
		return true; 
	}

	virtual update_status PreUpdate()
	{
		return update_status::UPDATE_CONTINUE;
	}

	virtual update_status Update()
	{
		return update_status::UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate()
	{
		return update_status::UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}
};

#endif // _MODULE_H_