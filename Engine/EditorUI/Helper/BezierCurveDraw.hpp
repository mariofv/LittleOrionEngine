#ifndef _BEZIER_CURVE_DRAW_HPP
#define _BEZIER_CURVE_DRAW_HPP

#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui.h>
#include <imgui_internal.h>
//#include <time.h>

#include <Helper/BezierCurve.h>

namespace ImGui
{
	static int const SMOOTHNESS = 64;
	static int const CURVE_WIDTH = 3;
	static int const LINE_WIDTH = 1;
	static int const GRAB_RADIUS = 5;
	static int const GRAB_BORDER = 1;
	//static bool const AREA_CONSTRAINED = true;
	static int const AREA_WIDTH = 0;

	static bool Bezier(BezierCurve* bezier)
	{
		// bezier widget
		const ImGuiStyle& Style = GetStyle();
		const ImGuiIO& IO = GetIO();
		ImDrawList* DrawList = GetWindowDrawList();
		ImGuiWindow* Window = GetCurrentWindow();
		if (Window->SkipItems)
			return false;

		// header and spacing
		bool changed = false;
		ImGui::Spacing();
		ImGui::Spacing();

		int hovered = IsItemActive() || IsItemHovered(); // IsItemDragged() ?
		Dummy(ImVec2(0, 3));

		// prepare canvas
		const float avail = GetContentRegionAvailWidth();
		const float dim = AREA_WIDTH > 0 ? AREA_WIDTH : avail;
		ImVec2 Canvas(dim, dim / 2);

		ImRect bb(Window->DC.CursorPos, Window->DC.CursorPos + Canvas);
		ItemSize(bb);
		if (!ItemAdd(bb, NULL))
			return changed;

		//const ImGuiID id = Window->GetID(label);
		//hovered |= 0 != ItemHoverable(ImRect(bb.Min, bb.Min + ImVec2(avail, dim / 2)), id);

		RenderFrame(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg, 1), true, Style.FrameRounding);

		// background grid
		for (int i = 0; i <= Canvas.x; i += (Canvas.x / 4)) {
			DrawList->AddLine(
				ImVec2(bb.Min.x + i, bb.Min.y),
				ImVec2(bb.Min.x + i, bb.Max.y),
				GetColorU32(ImGuiCol_TextDisabled));
		}
		for (int i = 0; i <= Canvas.y; i += (Canvas.y / 4)) {
			DrawList->AddLine(
				ImVec2(bb.Min.x, bb.Min.y + i),
				ImVec2(bb.Max.x, bb.Min.y + i),
				GetColorU32(ImGuiCol_TextDisabled));
		}

		// build curve
		float2 results[SMOOTHNESS];
		for (int i = 0; i < SMOOTHNESS; i++)
		{
			results[i] = bezier->BezierValue(float(i) / (float)SMOOTHNESS);
		}

		// handle grabbers
		ImVec2 mouse = GetIO().MousePos, pos;
		//int point_num = bezier.current_points * 3 - 2;
		float distance;
		pos = ImVec2(bezier->points[0].right_pivot.x, 1 - bezier->points[0].right_pivot.y) * (bb.Max - bb.Min) + bb.Min;
		distance = (pos.x - mouse.x) * (pos.x - mouse.x) + (pos.y - mouse.y) * (pos.y - mouse.y);
		if (distance < (4 * GRAB_RADIUS * 4 * GRAB_RADIUS))
		{
			//SetTooltip("(%4.3f, %4.3f)", P[selected * 2 + 0], P[selected * 2 + 1]);
			if (IsMouseClicked(0) || IsMouseDragging(0))
			{
				float &px = (bezier->points[0].right_pivot.x += GetIO().MouseDelta.x / Canvas.x);
				float &py = (bezier->points[0].right_pivot.y -= GetIO().MouseDelta.y / Canvas.y);

				changed = true;
			}
		}

		pos = ImVec2(bezier->points[1].left_pivot.x, 1 - bezier->points[1].left_pivot.y) * (bb.Max - bb.Min) + bb.Min;
		distance = (pos.x - mouse.x) * (pos.x - mouse.x) + (pos.y - mouse.y) * (pos.y - mouse.y);
		if (distance < (4 * GRAB_RADIUS * 4 * GRAB_RADIUS))
		{
			//SetTooltip("(%4.3f, %4.3f)", P[selected * 2 + 0], P[selected * 2 + 1]);
			if (IsMouseClicked(0) || IsMouseDragging(0))
			{
				float &px = (bezier->points[1].left_pivot.x += GetIO().MouseDelta.x / Canvas.x);
				float &py = (bezier->points[1].left_pivot.y -= GetIO().MouseDelta.y / Canvas.y);

				px = (px < 0 ? 0 : (px > 1 ? 1 : px));
				//py = (py < 0 ? 0 : (py > 1 ? 1 : py));

				changed = true;
			}
		}

		if (changed)
			bezier->CheckAllPointsAndPivots();

		// draw curve
		{
			ImColor color(GetStyle().Colors[ImGuiCol_PlotLines]);
			for (int i = 0; i < SMOOTHNESS - 1; i++) {
				ImVec2 p = { results[i + 0].x, 1 - results[i + 0].y };
				ImVec2 q = { results[i + 1].x, 1 - results[i + 1].y };
				ImVec2 r(p.x * (bb.Max.x - bb.Min.x) + bb.Min.x, p.y * (bb.Max.y - bb.Min.y) + bb.Min.y);
				ImVec2 s(q.x * (bb.Max.x - bb.Min.x) + bb.Min.x, q.y * (bb.Max.y - bb.Min.y) + bb.Min.y);
				DrawList->AddLine(r, s, color, CURVE_WIDTH);
			}
		}

		// draw lines and grabbers
		float luma = IsItemActive() || IsItemHovered() ? 0.5f : 1.0f;
		ImVec4 white(GetStyle().Colors[ImGuiCol_Text]);
		ImVec4 blue(0.15f, 0.23f, 0.40f, 0.7f);
		ImVec2 p1 = ImVec2(bezier->points[0].right_pivot.x, 1 - bezier->points[0].right_pivot.y) * (bb.Max - bb.Min) + bb.Min;
		ImVec2 p2 = ImVec2(bezier->points[1].left_pivot.x, 1 - bezier->points[1].left_pivot.y) * (bb.Max - bb.Min) + bb.Min;
		DrawList->AddLine(ImVec2(bb.Min.x, bb.Max.y), p1, ImColor(white), LINE_WIDTH / 2);
		DrawList->AddLine(ImVec2(bb.Max.x, bb.Min.y), p2, ImColor(white), LINE_WIDTH / 2);
		DrawList->AddCircleFilled(p1, GRAB_RADIUS, ImColor(white));
		DrawList->AddCircleFilled(p1, GRAB_RADIUS - GRAB_BORDER, ImColor(blue));
		DrawList->AddCircleFilled(p2, GRAB_RADIUS, ImColor(white));
		DrawList->AddCircleFilled(p2, GRAB_RADIUS - GRAB_BORDER, ImColor(blue));

		return changed;
	}

}
#endif //_BEZIER_CURVE_DRAW_HPP