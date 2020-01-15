#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_

#include <string>
#include <MathGeoLib.h>

class Billboard
{
public:
	Billboard();
	~Billboard();

	void Render(const std::string& texture_path, const float3& position) const;

private:
	float size = 10.f;

};

#endif //_BILLBOARD_H_