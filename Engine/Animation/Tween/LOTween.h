#ifndef _LOTWEEN_H_
#define _LOTWEEN_H_

#include "Animation/Tween/TweenSequence.h"

#include <vector>

static class LOTween
{
public:
	static TweenSequence* CreateSequence();

private:
	static void Update(float);

	static std::vector<TweenSequence*> sequences;

	friend class ModuleUI;
};
#endif _LOTWEEN_H_
