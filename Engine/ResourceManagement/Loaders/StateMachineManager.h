#ifndef _STATEMACHINEMANAGER_H_
#define _STATEMACHINEMANAGER_H_
#include <memory>
#include <string>
class StateMachine;
class StateMachineManager
{
public:
	StateMachineManager() = default;
	~StateMachineManager() = default;
	static std::shared_ptr<StateMachine> Load(const std::string& file_path);
};

#endif // !_H_STATEMACHINEMANAGER_

