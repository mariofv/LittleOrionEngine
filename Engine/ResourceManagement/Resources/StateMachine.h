#ifndef _H_STATEMACHINE_
#define _H_STATEMACHINE_

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
	long interpolation_time;
};
class StateMachine : Resource
{
public:
	StateMachine() = default;
	~StateMachine() = default;

public:
	std::vector<Clip> clips;
	std::vector<State> states;
	std::vector<Transition> transitions;
};

#endif // !_H_STATEMACHINE_


