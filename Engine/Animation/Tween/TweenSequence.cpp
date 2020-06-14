#include "TweenSequence.h"

TweenSequence* TweenSequence::Append(Tween* new_tween)
{
	float start_time = 0.0f;
	
	if (tweens.size() > 0)
	{
		Tween* last_tween = tweens.back();
		start_time = last_tween->start_time + last_tween->duration;
	}
	
	new_tween->start_time = start_time;
	tweens.push_back(new_tween);

	return this;
}

TweenSequence * TweenSequence::Join(Tween* new_tween)
{
	float start_time = 0.0f;
	
	if (tweens.size() > 0)
	{
		Tween* last_tween = tweens.back();
		start_time = last_tween->start_time;
	}
	
	new_tween->start_time = start_time;
	new_tween->duration += start_time;
	tweens.push_back(new_tween);

	return this;
}

TweenSequence * TweenSequence::Insert(float insert_time, Tween* new_tween)
{
	new_tween->start_time = insert_time;
	tweens.push_back(new_tween);

	return nullptr;
}

TweenSequence* TweenSequence::Play()
{
	state = TweenSequenceState::PLAYING;

	return this;
}

TweenSequence* TweenSequence::Stop()
{
	state = TweenSequenceState::STOPPED;
	tweens.clear();
	current_played_tweens.clear();
	return this;
}

TweenSequence * TweenSequence::Pause()
{
	state = TweenSequenceState::PAUSED;

	return this;
}

void TweenSequence::Update(float dt)
{
	if (state != TweenSequenceState::PLAYING) return;

	current_time += dt / 1000.0f;

	int pos = 0;
	for (std::vector<Tween*>::reverse_iterator it = tweens.rbegin(); it != tweens.rend(); ++it)
	{
		Tween* this_tween = (*it);
		
		if (this_tween->start_time < current_time)
		{
			//Is not the tween on the played tweens?
			if (std::find(current_played_tweens.begin(), current_played_tweens.end(), this_tween) == current_played_tweens.end()) 
			{
				this_tween->Play();
				tweens.erase(tweens.begin() + pos);
				current_played_tweens.push_back(this_tween);
				pos -= 1;
			}
		}

		pos+= 1;
	}

	pos = 0;
	for (std::vector<Tween*>::reverse_iterator it = current_played_tweens.rbegin(); it != current_played_tweens.rend(); ++it)
	{
		Tween* the_tween = (*it);
		the_tween->Update(dt);

		if (the_tween->state == Tween::TweenState::COMPLETED)
		{
			current_played_tweens.erase(current_played_tweens.begin() + pos);
			pos -= 1;

			delete(the_tween);
		}
		pos += 1;
	}

	if (current_played_tweens.size() <= 0)
	{
		Stop();
	}
}
