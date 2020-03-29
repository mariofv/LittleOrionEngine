#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_

#include "Resource.h"
#include "Animation.h"

struct Clip
{
	std::string name;
	std::shared_ptr<Animation> animation = nullptr;
	bool loop = false;
};

struct State
{
	std::string name;
	std::shared_ptr<Clip> clip = nullptr;
};

struct Transition
{
	std::string source;
	std::string target;
	std::string trigger;
	long interpolation_time = 0;
};
class StateMachine : Resource
{
public:
	StateMachine(const std::string & mesh_file_path);
	~StateMachine() = default;
	void LoadInMemory() override {};

	void Save() const;
public:
	std::vector<std::shared_ptr<Clip>> clips;
	std::vector<std::shared_ptr<State>> states;
	std::vector<std::shared_ptr<Transition>> transitions;
};

#endif // !_H_STATEMACHINE_


