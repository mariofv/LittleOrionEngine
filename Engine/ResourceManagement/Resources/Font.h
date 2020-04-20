#ifndef _FONT_H_
#define _FONT_H_

#include "Resource.h"

class Font : public Resource
{
public:
	Font() = default;
	Font(uint32_t uuid);

	~Font() = default;

private:
	uint32_t uuid = 0;
};

namespace ResourceManagement
{
	template<>
	static std::shared_ptr<Font> Load(uint32_t uuid, const FileData& resource_data)
	{
		return nullptr;
	};
}

#endif // !_RESOURCE_H_

