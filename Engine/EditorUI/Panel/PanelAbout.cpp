#include "PanelAbout.h"

#include "Main/Application.h"

#include <FontAwesome5/IconsFontAwesome5.h>
#include <imgui.h>

PanelAbout::PanelAbout()
{
	window_name = ICON_FA_QUESTION_CIRCLE " About";
}

void PanelAbout::Render()
{
	if (ImGui::Begin(window_name.c_str(), &opened))
	{
		hovered = ImGui::IsWindowHovered();
		focused = ImGui::IsWindowFocused();

		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "LittleOrion Engine");
		ImGui::TextWrapped("3D engine developed during the Master's Degree in AAA Videogames Development.");


		ImGui::Separator();


		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Authors:");
		ImGui::Text("Pau Casas Garcia");
		ImGui::Text("Mario Fernandez Villalba");
		ImGui::Text("Toni Ferrari Juan");
		ImGui::Text("Artemis Georgakopoulou");
		ImGui::Text("Joan Ginard Mateo");
		ImGui::Text("Raul Gonzalez Cebrian");
		ImGui::Text("Enrique Alexandre Gonzalez Sequeira");
		ImGui::Text("Anabel Hernandez Barrera");
		ImGui::Text("Vidal Melero Iglesias");
		ImGui::Text("Luis Carlos Moyano Medina");
		ImGui::Text("Marco Rodriguez Quinto");
		ImGui::Text("Jordi Romagosa Mellado");
		ImGui::Text("Jordi Sauras Salas");
		ImGui::Text("Ricard Vivo Montero");


		ImGui::Separator();


		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Libraries");

		MenuURL("SDL 2.0", "https://www.libsdl.org/");
		MenuURL("glew 2.1.0", "http://glew.sourceforge.net/");
		MenuURL("MathGeoLib 1.5", "https://github.com/juj/MathGeoLib/");
		MenuURL("ImGui 1.73", "https://github.com/ocornut/imgui/");
		MenuURL("DevIL 1.8", "http://openil.sourceforge.net/");
		MenuURL("assimp 5.0", "https://github.com/assimp/assimp/");
		MenuURL("Font Awesome 5", "https://github.com/FortAwesome/Font-Awesome/");
		MenuURL("Icon Font Cpp Headers", "https://github.com/juliettef/IconFontCppHeaders/");
		MenuURL("PCG", "http://www.pcg-random.org/");
		MenuURL("rapidjson 1.1.0", "https://github.com/Tencent/rapidjson/");
		MenuURL("Debug Draw", "https://github.com/glampert/debug-draw/");
		MenuURL("par_shapes", "https://github.com/prideout/par/blob/master/par_shapes.h");
		MenuURL("ImGuizmo", "https://github.com/CedricGuillemet/ImGuizmo");
		MenuURL("PhysFS 3.0.2", "http://icculus.org/physfs/");
		MenuURL("cr", "https://github.com/fungos/cr");
		MenuURL("spdlog", "https://github.com/gabime/spdlog");

		ImGui::Separator();

		MenuURL("LICENSE", "https://github.com/OnionGalaxy/LittleOrionEngine/blob/master/LICENSE");
		ImGui::TextWrapped("LittleOrion Engine is licensed under the MIT License, see LICENSE for more information.");
	}
	ImGui::End();
}

void PanelAbout::MenuURL(const std::string& text, const std::string& link)
{
	if (ImGui::MenuItem(text.c_str()))
	{
		ShellExecuteA(NULL, "open", link.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}
}