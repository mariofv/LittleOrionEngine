#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_

#include "Resource.h"
#include "Animation.h"

struct Clip
{
	Clip(std::string name, std::shared_ptr<Animation> & animation, bool loop) :
		name(name), animation(animation), loop(loop) {};
	std::string name;
	std::shared_ptr<Animation> animation = nullptr;
	bool loop = false;
};

struct State
{
	State(std::string & name, std::shared_ptr<Clip> & clip) :
		name(name), clip(clip) {};
	std::string name;
	std::shared_ptr<Clip> clip = nullptr;
};

struct Transition
{
	Transition(std::string & source, std::string & target, std::string & trigger, long interpolation) :
		source(source), target(target), trigger(trigger), interpolation_time(interpolation) {};
	std::string source;
	std::string target;
	std::string trigger;
	long interpolation_time = 0;
};
class File;
class StateMachine : Resource
{
public:
	StateMachine(const std::string & file_path);
	~StateMachine() = default;
	void LoadInMemory() override {};

	void Save() const;
	void Load(const File & file);
public:
	std::vector<std::shared_ptr<Clip>> clips;
	std::vector<std::shared_ptr<State>> states;
	std::vector<std::shared_ptr<Transition>> transitions;
};

#endif // !_H_STATEMACHINE_


