#ifndef _LOTWEEN_H_
#define _LOTWEEN_H_
#define ENGINE_EXPORTS

#include "TweenSequence.h"
#include <vector>

class LOTween
{
public:
	ENGINE_API TweenSequence* CreateSequence();
	void Update(float);
	void Reset();

private:
	std::vector<TweenSequence*> sequences;
};
#endif _LOTWEEN_H_
