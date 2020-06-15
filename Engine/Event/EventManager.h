#ifndef _EVENTMANAGER_H_
#define _EVENTMANAGER_H_

#include "Event/FunctionHandler.h"

#include<map>
#include<typeindex>
#include<vector>

typedef std::vector<BaseFunctionHandler*> HandlerList;

class EventManager
{
public:
	template<typename EventType>
	void publish(EventType* event)
	{
		HandlerList* handlers = subscribers[typeid(EventType)];

		if (handlers == nullptr)
		{
			return;
		}

		for (auto& handler : *handlers)
		{
			if (handler != nullptr && event->emitter == handler->suscriber)
			{
				handler->exec(event);
			}
		}
	}

	template<class T, class EventType>
	void subscribe(T* instance, GameObject* suscriber, void (T::*memberFunction)(EventType *))
	{
		HandlerList* handlers = subscribers[typeid(EventType)];

		//First time initialization
		if (handlers == nullptr)
		{
			handlers = new HandlerList();
			subscribers[typeid(EventType)] = handlers;
		}

		handlers->push_back(new FunctionHandler<T, EventType>(instance, suscriber, memberFunction));
	}
private:
	std::map<std::type_index, HandlerList*> subscribers;
};

#endif
