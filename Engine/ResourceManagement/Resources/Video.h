#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <GL/glew.h>
#include "Resource.h"

namespace cv
{
	class VideoCapture;
}

class Video : public Resource
{

public:
	Video(uint32_t uuid, bool async = false);
	~Video();

	void LoadInMemory();

	GLuint GenerateFrame();
	void Stop();

private:
	GLuint opengl_texture = 0;
	cv::VideoCapture* video_capture = nullptr;
	std::string file_path;

	float frame_time;
	float frame_timer = 0.0f;
};

namespace ResourceManagement
{
	template<>
	static std::shared_ptr<Video> Load(uint32_t uuid, const FileData& resource_data, bool async)
	{
		return std::make_shared<Video>(uuid, async);
	}

}
#endif
