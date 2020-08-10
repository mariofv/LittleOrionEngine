#include "Video.h"

#include "ResourceManagement/Metafile/MetafileManager.h"
#include "Log/EngineLog.h"
#include "Main/Application.h"
#include "Module/ModuleTime.h"
#include "opencv2/opencv.hpp"

Video::Video(uint32_t uuid, bool async) : Resource(uuid)
{
	if (!async)
	{
		LoadInMemory();
	}
}

Video::~Video()
{
	video_capture->release();

}

void Video::LoadInMemory()
{
	file_path = MetafileManager::GetUUIDExportedFile(uuid);
	file_path = file_path.substr(file_path.find_first_of("/") + 1, file_path.size());

	glGenTextures(1, &opengl_texture);
	glBindTexture(GL_TEXTURE_2D, opengl_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	video_capture = new cv::VideoCapture(file_path.c_str());
	// Check if video opened successfully
	if (!video_capture->isOpened()) {
		APP_LOG_ERROR("Error opening video");
	}
	else
	{
		frame_time = 1.0f / video_capture->get(cv::CAP_PROP_FPS);
	}
}

GLuint Video::GenerateFrame()
{
	frame_timer += App->time->delta_time/1000.0f;
	if (frame_timer > frame_time)
	{	
		cv::Mat frame;
		do
		{
			frame_timer -= frame_time;
			*video_capture >> frame;
		} while (frame_timer >= frame_time);

		if (!frame.empty())
		{
			cv::Mat flipped;
			cv::flip(frame, flipped, 0);
			glBindTexture(GL_TEXTURE_2D, opengl_texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, flipped.ptr());
			glBindTexture(GL_TEXTURE_2D, 0);
			return opengl_texture;
		}
		else
		{
			Stop();
			return 0;
		}
	}
	return opengl_texture;
}

void Video::Stop()
{
	video_capture->set(cv::CAP_PROP_POS_FRAMES, 0);
	frame_timer = 0.0f;
}

