#include "ModuleSpritesheet.h"

ModuleSpritesheet::~ModuleSpritesheet() {

}

bool ModuleSpritesheet::Init() {
	APP_LOG_SECTION("************ Module Spritesheet Init ************");
	return true;
}

void ModuleSpritesheet::Render(const float3& position, GLuint program) {

	billboard.Render(position);

}

bool ModuleSpritesheet::CleanUp() {
	APP_LOG_SECTION("************ Module Spritesheet CleanUp ************");
	return true;
}