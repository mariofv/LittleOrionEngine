#include "LOTween.h"
#include "TweenSequence.h"
#include <vector>

TweenSequence* LOTween::CreateSequence()
{
	TweenSequence* sequence = new TweenSequence();
	sequence->state = TweenSequence::TweenSequenceState::DISABLED;
	sequences.push_back(sequence);

	return sequence;
}

void LOTween::Update(float dt)
{
	if (sequences.size() <= 0 || dt >= MAX_TIME_ALLOWED) return;

	for (std::vector<TweenSequence*>::reverse_iterator it = sequences.rbegin(); it != sequences.rend(); ++it)
	{
		TweenSequence* sequence = (*it);
		sequence->Update(dt);
	}
}

void LOTween::Reset()
{
	for (std::vector<TweenSequence*>::reverse_iterator it = sequences.rbegin(); it != sequences.rend(); ++it)
	{
		TweenSequence* sequence = (*it);
		(*it)->Clear();
		delete(sequence);
	}

	sequences.clear();
}

void LOTween::CleanSequences()
{
	for (std::vector<TweenSequence*>::iterator it = sequences.begin(); it != sequences.end();)
	{
		TweenSequence* sequence = (*it);
		if (sequence->state == TweenSequence::TweenSequenceState::STOPPED)
		{
			it = sequences.erase(it);
			delete(sequence);
		}
		else
		{
			++it;
		}
	}
}
