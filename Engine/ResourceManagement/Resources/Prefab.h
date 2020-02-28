#ifndef _PREFAB_H_
#define _PREFAB_H_

#include "Resource.h"

#include <vector>
#include <memory>
class GameObject;
class Prefab : public Resource
{
public:
	Prefab() = default;
	void Instantiate(const GameObject * prefab_parent);

private:
	std::vector<std::unique_ptr<GameObject>> prefab;
};

#endif // !_PREFAB_H_



