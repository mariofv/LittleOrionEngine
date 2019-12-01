#ifndef _COMPONENTCAMERA_H_
#define _COMPONENTCAMERA_H_

#include "Component.h"

#include "MathGeoLib.h"
#include <GL/glew.h>

class ComponentCamera : public Component
{
public:
	ComponentCamera(GameObject * owner);

	~ComponentCamera() = default;

	void Enable() override;
	void Disable() override;
	void Update() override;

	void ShowComponentWindow() override;

private:
	
};

#endif //_COMPONENTCAMERA_H_