#include "Video.h"

#include "ResourceManagement/Metafile/MetafileManager.h"
#include "Main/Application.h"
#include "opencv2/opencv.hpp"

Video::Video(uint32_t uuid) : Resource(uuid)
{
	file_path = MetafileManager::GetUUIDExportedFile(uuid);
}

Video::~Video()
{
	video_capture->release();
	cv::destroyAllWindows();
}

void Video::Init()
{
	video_capture = new cv::VideoCapture(file_path.c_str());
	// Check if video opened successfully
	if (!video_capture->isOpened()) {
		//APP_LOG_ERROR("Error opening video");
	}
}

void Video::Update()
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
		cv::imshow("Frame", frame);
	}
}
void Video::Play()
{
	playing = true;
}
