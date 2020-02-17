#ifndef _COMPONENTSCRIPT_H_
#define _COMPONENTSCRIPT_H_
#include "Component.h"
class Application;

class ComponentScript :public Component
{
public:
	ComponentScript();
	ComponentScript(GameObject*);
	~ComponentScript();
	virtual void Start() {};
	virtual void Awake() {};
	virtual void Update() {};
	virtual void OnInspector() {};
	void SetAppPointer(Application*);
	//TODO: Implement more global functions Unity-like
public:

	Application *App = nullptr;
};
#endif //_COMPONENTSCRIPT_H_

