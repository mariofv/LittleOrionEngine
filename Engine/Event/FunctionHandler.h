#ifndef _FUNCTIONHANDLER_H_
#define _FUNCTIONHANDLER_H_

#include "Event.h"
#include "Main/GameObject.h"

class BaseFunctionHandler {
public:
	BaseFunctionHandler(GameObject* suscriber) : suscriber(suscriber) {}
	// Call the member function
	void Execute(Event* evnt)
	{
		Call(evnt);
	}
public:
	GameObject* suscriber;

private:
	// Implemented by FunctionHandler
	virtual void Call(Event* evnt) = 0;
};

template<class T, class EventType>
class FunctionHandler : public BaseFunctionHandler
{
public:
	typedef void (T::*MemberFunction)(EventType*);

	FunctionHandler(T* instance, GameObject* suscriber, MemberFunction member_function) : BaseFunctionHandler(suscriber),
		instance(instance), member_function(member_function) {};

	void Call(Event* evnt)
	{
		// Cast event to the correct type and call member function
		(instance->*member_function)(static_cast<EventType*>(evnt));
	}
private:
	// Pointer to class instance
	T* instance;

	// Pointer to member function
	MemberFunction member_function;
};
#endif
