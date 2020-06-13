#ifndef _LOTWEEN_H_
#define _LOTWEEN_H_

#include "TweenSequence.h"
#include <vector>

class LOTween
{
public:
	TweenSequence* CreateSequence();

private:
	void Update(float);

	std::vector<TweenSequence*> sequences;

	friend class ModuleUI;
};
#endif _LOTWEEN_H_
