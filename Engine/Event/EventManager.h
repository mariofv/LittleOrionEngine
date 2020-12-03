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
	void Publish(EventType* event)
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
				handler->Execute(event);
				if (event)
				{
					delete event;
				}
			}
		}
	}

	template<class T, class EventType>
	void Subscribe(T* instance, GameObject* suscriber, void (T::*memberFunction)(EventType *))
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

	template<class T, class EventType>
	void Unsubscribe(T* instance, GameObject* suscriber, void (T::*memberFunction)(EventType *))
	{
		HandlerList* handlers = subscribers[typeid(EventType)];

		if (handlers == nullptr)
		{
			return;
		}

		BaseFunctionHandler* handler_to_delete = nullptr;
		for (auto& handler : *handlers)
		{
			if (suscriber == handler->suscriber)
			{
				handler_to_delete = handler;
			}
		}

		auto it = std::find(handlers->begin(), handlers->end(), handler_to_delete);
		if (it != handlers->end())
		{
			delete *it;
			handlers->erase(it);
		}

	}

private:
	std::map<std::type_index, HandlerList*> subscribers;
};

#endif
