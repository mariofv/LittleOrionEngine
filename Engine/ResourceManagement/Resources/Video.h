#ifndef _COMPONENVIDEO_H_
#define _Video_H_

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

	void Update();

	void Play();

private:
	cv::VideoCapture* video_capture = nullptr;
	bool playing = false;
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
