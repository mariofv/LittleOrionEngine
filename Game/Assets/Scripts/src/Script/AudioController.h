#ifndef  __AUDIOCONTROLLER_H__
#define  __AUDIOCONTROLLER_H__

#include "Script.h"

class AudioController : public Script
{
public:
	AudioController();
	~AudioController() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	//void Save(Config& config) const override;
	//void Load(const Config& config) override;


};
extern "C" SCRIPT_API AudioController* AudioControllerDLL(); //This is how we are going to load the script
#endif