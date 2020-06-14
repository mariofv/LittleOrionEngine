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
	int pos = 0;
	for (std::vector<TweenSequence*>::reverse_iterator it = sequences.rbegin(); it != sequences.rend(); ++it)
	{
		TweenSequence* sequence = (*it);
		sequence->Update(dt);

		if (sequence->state == TweenSequence::TweenSequenceState::STOPPED)
		{
			sequences.erase(sequences.begin() + pos);
			pos -= 1;

			delete(sequence);
		}
		pos += 1;
	}
}
