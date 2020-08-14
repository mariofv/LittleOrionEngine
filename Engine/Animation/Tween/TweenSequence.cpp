#include "TweenSequence.h"

void TweenSequence::Clear()
{
	for (auto it = tweens.begin(); it != tweens.end(); ++it)
	{
		delete *it;
	}

	tweens.clear();
}

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

TweenSequence* TweenSequence::Join(Tween* new_tween)
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

TweenSequence* TweenSequence::Insert(float insert_time, Tween* new_tween)
{
	new_tween->start_time = insert_time;
	tweens.push_back(new_tween);

	return this;
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

TweenSequence* TweenSequence::Pause()
{
	state = TweenSequenceState::PAUSED;

	return this;
}

TweenSequence* TweenSequence::OnCompleted(std::function<void(void)> callback)
{
	on_completed_callback = callback;
	return this;
}

void TweenSequence::Update(float dt)
{
	if (state != TweenSequenceState::PLAYING) return;

	if(dt == 0.f)
	{
		return;
	}

	current_time += dt * 0.001f;

	for (size_t i = 0; i < tweens.size(); i++)
	{
		Tween* this_tween = tweens.at(i);
		
		if (this_tween->start_time < current_time)
		{
			//Is not the tween on the played tweens?
			if (std::find(current_played_tweens.begin(), current_played_tweens.end(), this_tween) == current_played_tweens.end()) 
			{
				this_tween->Play();
				tweens.erase(tweens.begin() + i);
				current_played_tweens.push_back(this_tween);
				i -= 1;
			}
		}
	}

	for (size_t i = 0; i < current_played_tweens.size(); i++)
	{
		Tween* the_tween = current_played_tweens.at(i);
		the_tween->Update(dt);

		if (the_tween->state == Tween::TweenState::COMPLETED)
		{
			current_played_tweens.erase(current_played_tweens.begin() + i);
			i -= 1;
		}
	}

	if (current_played_tweens.size() <= 0)
	{
		if (on_completed_callback != nullptr)
		{
			on_completed_callback();
		}

		Stop();
	}
}
