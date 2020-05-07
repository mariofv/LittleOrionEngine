#ifndef  __MUSHDOOM_H__
#define  __MUSHDOOM_H__

#include "Script.h"
#include "EnemyController.h"

class Mushdoom : public EnemyController
{
public:
	Mushdoom();
	~Mushdoom() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	//void OnInspector(ImGuiContext*) override;
	//void InitPublicGameObjects();

private:
};

extern "C" SCRIPT_API Mushdoom* MushdoomDLL(); //This is how we are going to load the script

#endif