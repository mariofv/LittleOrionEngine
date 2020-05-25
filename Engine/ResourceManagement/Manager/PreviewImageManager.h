#ifndef _PANELPROJECTEXPLORER_H_
#define _PANELPROJECTEXPLORER_H_
class PreviewImageManager
{
public:
	PreviewImageManager() = default;
	~PreviewImageManager() = default;

	static void TakeScreenShot(const char * screenshot_save_path);
};

#endif
