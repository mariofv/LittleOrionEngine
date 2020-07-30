#ifndef _COMPONENVIDEO_H_
#define _COMPONENTVIDEO_H_

#include "Component/Component.h"
class GameObject;
namespace cv
{
	class VideoCapture;
}
class ComponentVideo : public Component
{

public:
	ComponentVideo();
	ComponentVideo(GameObject* owner);
	~ComponentVideo();

	void Init();

	void Update();

	void Play();

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;

	void Delete() override;
	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

private:
	cv::VideoCapture* video_capture = nullptr;
	bool playing = false;
};
#endif
