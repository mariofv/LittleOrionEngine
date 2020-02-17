#ifndef  __TESTSCRIPT_H__
#define  __TESTSCRIPT_H__
#include "../../Engine/Component/ComponentScript.h"

#ifdef TESTSCRIPT_EXPORTS
#define TESTSCRIPT_API __declspec(dllexport)
#else
#define TESTSCRIPT_API __declspec(dllimport)
#endif
class TESTSCRIPT_API TestScript : public ComponentScript 
{
	void Test();
};
#endif //__TESTSCRIPT_H__