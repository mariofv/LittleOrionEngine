#ifndef _FUNCTIONHANDLER_H_
#define _FUNCTIONHANDLER_H_

#include "Event.h"

class BaseFunctionHandler {
public:
	// Call the member function
	void exec(Event* evnt)
	{
		call(evnt);
	}
private:
	// Implemented by FunctionHandler
	virtual void call(Event* evnt) = 0;
};

template<class T, class EventType>
class FunctionHandler : public BaseFunctionHandler
{
public:
	typedef void (T::*MemberFunction)(EventType*);

	FunctionHandler(T* instance, MemberFunction memberFunction) : instance{ instance }, memberFunction{ memberFunction } {};

	void call(Event* evnt)
	{
		// Cast event to the correct type and call member function
		(instance->*memberFunction)(static_cast<EventType*>(evnt));
	}
private:
	// Pointer to class instance
	T * instance;

	// Pointer to member function
	MemberFunction memberFunction;
};
#endif
