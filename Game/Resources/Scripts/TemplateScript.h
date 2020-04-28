#ifndef  __TEMPLATESCRIPT_H__
#define  __TEMPLATESCRIPT_H__

#include "Script.h"

class TemplateScript : public Script
{
public:
	TemplateScript();
	~TemplateScript() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	//void Save(Config& config) const override;
	//void Load(const Config& config) override;

private:
	GameObject* example = nullptr;

};
extern "C" SCRIPT_API TemplateScript* TemplateScriptDLL(); //This is how we are going to load the script
#endif