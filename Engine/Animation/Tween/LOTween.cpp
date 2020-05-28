#include "LOTween.h"

TweenSequence* LOTween::CreateSequence()
{
	TweenSequence* sequence = new TweenSequence();
	sequence->state = TweenSequence::TweenSequenceState::DISABLED;
	sequences.push_back(sequence);

	return sequence;
}

void LOTween::Update(float dt)
{
	for (std::vector<TweenSequence*>::reverse_iterator it = sequences.rbegin(); it != sequences.rend(); ++it)
	{
		(*it)->Update(dt);
	}
}
