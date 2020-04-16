#ifndef _STATEMACHINEMANAGER_H_
#define _STATEMACHINEMANAGER_H_

#include "Filesystem/File.h"

#include <memory>
#include <string>

class StateMachine;
class Metafile;
class Path;

class StateMachineManager

{
public:
	StateMachineManager() = default;
	~StateMachineManager() = default;
	static std::shared_ptr<StateMachine> Load(Metafile* metafile, const FileData& resource_data);
};

#endif // !_H_STATEMACHINEMANAGER_

