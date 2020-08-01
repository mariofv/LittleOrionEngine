#include "Video.h"

#include "ResourceManagement/Metafile/MetafileManager.h"
#include "Log/EngineLog.h"
#include "opencv2/opencv.hpp"

Video::Video(uint32_t uuid) : Resource(uuid)
{
	file_path = MetafileManager::GetUUIDExportedFile(uuid);
	Init();
}

Video::~Video()
{
	video_capture->release();
	cv::destroyAllWindows();
}

void Video::Init()
{
	glGenTextures(1, &opengl_texture);
	glBindTexture(GL_TEXTURE_2D, opengl_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	video_capture = new cv::VideoCapture("Assets/Video/twinlight_souls.mp4");
	// Check if video opened successfully
	if (!video_capture->isOpened()) {
		APP_LOG_ERROR("Error opening video");
	}
}

bool Video::Update()
{

	cv::Mat frame;
	*video_capture >> frame;

	if (!frame.empty())
	{
		cv::Mat flipped;
		cv::flip(frame, flipped, 0);
		glBindTexture(GL_TEXTURE_2D, opengl_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, flipped.ptr());
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	return !frame.empty();
}

