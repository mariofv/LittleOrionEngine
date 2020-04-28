#include "SoundBank.h"

SoundBank::SoundBank(uint32_t uuid, const void* data, size_t image_size) : Resource(uuid)
{
	this->data.resize(image_size);
	memcpy(&this->data.front(), data, image_size);
	AK::SoundEngine::LoadBankMemoryView(this->data.data(), image_size, banck_id);
}
SoundBank::~SoundBank()
{
	AK::SoundEngine::UnloadBank(banck_id, this->data.data());
}