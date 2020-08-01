#ifndef _COMPONENVIDEO_H_
#define _Video_H_

#include <GL/glew.h>
#include "Resource.h"

namespace cv
{
	class VideoCapture;
}

class Video : public Resource
{

public:
	Video(uint32_t uuid);
	~Video();

	void Init();

	bool Update();
	GLuint opengl_texture = 0;

private:
	cv::VideoCapture* video_capture = nullptr;
	std::string file_path;
};

namespace ResourceManagement
{
	template<>
	static std::shared_ptr<Video> Load(uint32_t uuid, const FileData& resource_data, bool async)
	{
		return std::make_shared<Video>(uuid);
	}

}
#endif
