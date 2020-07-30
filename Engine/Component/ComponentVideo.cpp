#include "ComponentVideo.h"

#include "Main/GameObject.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleUI.h"

#include "opencv2/opencv.hpp"

ComponentVideo::ComponentVideo() : Component(ComponentType::VIDEO)
{

}

ComponentVideo::ComponentVideo(GameObject * owner) : Component(owner, ComponentType::VIDEO)
{
}

ComponentVideo::~ComponentVideo()
{
	video_capture->release();
	cv::destroyAllWindows();
}

void ComponentVideo::Init()
{
	video_capture = new cv::VideoCapture("assets/video/twinlight_souls.mp4");
	// Check if video opened successfully
	if (!video_capture->isOpened()) {
		APP_LOG_ERROR("Error opening video");
	}
}

void ComponentVideo::Update()
{
	if (playing)
	{
		cv::Mat frame;
		*video_capture >> frame;

		playing = !frame.empty();
		if (!playing)
		{
			return;
		}
		imshow("Frame", frame);
	}
}
void ComponentVideo::Play()
{
	playing = true;
}

Component* ComponentVideo::Clone(bool original_prefab) const
{
	ComponentVideo * created_component;
	if (original_prefab)
	{
		created_component = new ComponentVideo();
	}
	else
	{
		created_component = App->ui->CreateComponentUI<ComponentVideo>();
	}
	*created_component = *this;
	CloneBase(static_cast<Component*>(created_component));

	return created_component;
};

void ComponentVideo::Copy(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentVideo*>(component_to_copy) = *this;
}


void ComponentVideo::Delete()
{
	App->ui->RemoveComponentUI(this);
}


void ComponentVideo::SpecializedSave(Config& config) const
{

}

void ComponentVideo::SpecializedLoad(const Config& config)
{

}