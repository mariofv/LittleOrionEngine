#ifndef _LOTWEEN_H_
#define _LOTWEEN_H_
#ifndef ENGINE_EXPORTS
#define ENGINE_EXPORTS
#endif

#include "TweenSequence.h"
#include <vector>

class LOTween
{
public:
	ENGINE_API TweenSequence* CreateSequence();
	void Update(float);
	void Reset();
	void CleanSequences();

private:
	std::vector<TweenSequence*> sequences;
};
#endif _LOTWEEN_H_
