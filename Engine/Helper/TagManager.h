#ifndef __TAGMANAGER_H__
#define __TAGMANAGER_H__

#include <string>
#include <vector>

class TagManager
{
public:
	TagManager() = default;
	~TagManager() = default;

	void AddTag(const std::string& new_tag);
	void RemoveTag(const std::string& tag_to_remove);

	bool LoadTags();
	bool SaveTags() const;

	std::vector<std::string> tags;
};

#endif //__TAGMANAGER_H__

