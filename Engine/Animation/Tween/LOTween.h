#ifndef _LOTWEEN_H_
#define _LOTWEEN_H_
#define ENGINE_EXPORTS

#include "TweenSequence.h"
#include <vector>

class LOTween
{
public:
	ENGINE_API TweenSequence* CreateSequence();

private:
	void Update(float);

	std::vector<TweenSequence*> sequences;

	friend class ModuleUI;
};
#endif _LOTWEEN_H_
