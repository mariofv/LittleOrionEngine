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
	static int const GRAB_RADIUS = 4;
	static int const GRAB_BORDER = 1;
	static int const POINT_RADIUS = 4;
	static int const POINT_VERTEXS = 4;
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
		float2 results[SMOOTHNESS + 1];
		for (int i = 0; i <= SMOOTHNESS; i++)
		{
			results[i] = bezier->BezierValue(float(i) / (float)SMOOTHNESS);
		}

		// handle and point grabbers
		{
			ImVec2 mouse = GetIO().MousePos, pos[3 * bezier->MAXIMUM_POINTS];
			float total_points_and_pivots = bezier->num_points * 3 - 2;

			for (int i = 0; i < bezier->num_points; i++)
			{
				if (i == 0)
				{
					pos[0] = ImVec2(bezier->points[i].curve_point.x, 1 - bezier->points[i].curve_point.y) * (bb.Max - bb.Min) + bb.Min;
					pos[1] = ImVec2(bezier->points[i].right_pivot.x, 1 - bezier->points[i].right_pivot.y) * (bb.Max - bb.Min) + bb.Min;
				}

				else if (i == bezier->num_points - 1)
				{
					pos[2 + (i - 1) * 3 + 0] = ImVec2(bezier->points[i].left_pivot.x, 1 - bezier->points[i].left_pivot.y) * (bb.Max - bb.Min) + bb.Min;
					pos[2 + (i - 1) * 3 + 1] = ImVec2(bezier->points[i].curve_point.x, 1 - bezier->points[i].curve_point.y) * (bb.Max - bb.Min) + bb.Min;
				}

				else
				{
					pos[2 + (i - 1) * 3 + 0] = ImVec2(bezier->points[i].left_pivot.x, 1 - bezier->points[i].left_pivot.y) * (bb.Max - bb.Min) + bb.Min;
					pos[2 + (i - 1) * 3 + 1] = ImVec2(bezier->points[i].curve_point.x, 1 - bezier->points[i].curve_point.y) * (bb.Max - bb.Min) + bb.Min;
					pos[2 + (i - 1) * 3 + 2] = ImVec2(bezier->points[i].right_pivot.x, 1 - bezier->points[i].right_pivot.y) * (bb.Max - bb.Min) + bb.Min;
				}
			}

			float min_distance = 1000000;
			int point_clicked = 0, point_side = 0; //side: 0=center, -1=left, 1=right
			for (int i = 0; i < total_points_and_pivots; i++)
			{
				float distance = (pos[i].x - mouse.x) * (pos[i].x - mouse.x) + (pos[i].y - mouse.y) * (pos[i].y - mouse.y);
				if (distance < min_distance)
				{
					min_distance = distance;

					if (i < 2)
					{
						point_clicked = 0;
						point_side = i;
					}
					else if (i > total_points_and_pivots - 2)
					{
						point_clicked = bezier->num_points - 1;
						point_side = (i + 1) - total_points_and_pivots;
					}
					else
					{
						point_clicked = (i - ((i - 2) % 3)) / 3 + 1;
						point_side = (i - 2) % 3 - 1;
					}
				}
			}
			
			//add or remove points
			int detection_distance = 4 * GRAB_RADIUS * 4 * GRAB_RADIUS;
			if (IsMouseDoubleClicked(0))
			{
				if (min_distance < detection_distance && point_side == 0)
					bezier->RemovePointWithIndex(point_clicked);
				else
					bezier->AddPointAtCurve((mouse.x - bb.Min.x) / (bb.Max.x - bb.Min.x));
			}

			//drag and move points
			if (min_distance < detection_distance)
			{
				if (IsMouseClicked(0) || IsMouseDragging(0))
				{
					if (point_side == -1)
					{
						bezier->points[point_clicked].left_pivot.x += GetIO().MouseDelta.x / Canvas.x;
						bezier->points[point_clicked].left_pivot.y -= GetIO().MouseDelta.y / Canvas.y;
					}
					else if (point_side == 0)
					{
						bezier->points[point_clicked].curve_point.x += GetIO().MouseDelta.x / Canvas.x;
						bezier->points[point_clicked].curve_point.y -= GetIO().MouseDelta.y / Canvas.y;
					}
					else
					{
						bezier->points[point_clicked].right_pivot.x += GetIO().MouseDelta.x / Canvas.x;
						bezier->points[point_clicked].right_pivot.y -= GetIO().MouseDelta.y / Canvas.y;
					}

					bezier->CheckAllPointsAndPivots();
					changed = true;
				}
			}
		}

		// draw curve
		{
			ImColor color(GetStyle().Colors[ImGuiCol_PlotLines]);
			for (int i = 0; i <= SMOOTHNESS - 1; i++) {
				ImVec2 p = { results[i + 0].x, 1 - results[i + 0].y };
				ImVec2 q = { results[i + 1].x, 1 - results[i + 1].y };
				ImVec2 r(p.x * (bb.Max.x - bb.Min.x) + bb.Min.x, p.y * (bb.Max.y - bb.Min.y) + bb.Min.y);
				ImVec2 s(q.x * (bb.Max.x - bb.Min.x) + bb.Min.x, q.y * (bb.Max.y - bb.Min.y) + bb.Min.y);
				DrawList->AddLine(r, s, color, CURVE_WIDTH);
			}
		}

		//draw curve points and grabbers
		ImVec4 white(GetStyle().Colors[ImGuiCol_Text]);
		ImVec4 blue(0.15f, 0.23f, 0.40f, 0.7f);
		for (int i = 0; i < bezier->num_points; i++)
		{
			ImVec2 p = ImVec2(bezier->points[i].curve_point.x, 1 - bezier->points[i].curve_point.y) * (bb.Max - bb.Min) + bb.Min;
			DrawList->AddNgonFilled(p, POINT_RADIUS, ImColor(white), POINT_VERTEXS);

			if (i == 0)
			{
				ImVec2 h = ImVec2(bezier->points[i].right_pivot.x, 1 - bezier->points[i].right_pivot.y) * (bb.Max - bb.Min) + bb.Min;
				DrawList->AddLine(p, h, ImColor(white), LINE_WIDTH / 2);
				DrawList->AddCircleFilled(h, GRAB_RADIUS, ImColor(white));
				DrawList->AddCircleFilled(h, GRAB_RADIUS - GRAB_BORDER, ImColor(blue));
			}

			else if (i == bezier->num_points - 1)
			{
				ImVec2 h = ImVec2(bezier->points[i].left_pivot.x, 1 - bezier->points[i].left_pivot.y) * (bb.Max - bb.Min) + bb.Min;
				DrawList->AddLine(p, h, ImColor(white), LINE_WIDTH / 2);
				DrawList->AddCircleFilled(h, GRAB_RADIUS, ImColor(white));
				DrawList->AddCircleFilled(h, GRAB_RADIUS - GRAB_BORDER, ImColor(blue));
			}

			else
			{
				ImVec2 h2 = ImVec2(bezier->points[i].left_pivot.x, 1 - bezier->points[i].left_pivot.y) * (bb.Max - bb.Min) + bb.Min;
				ImVec2 h1 = ImVec2(bezier->points[i].right_pivot.x, 1 - bezier->points[i].right_pivot.y) * (bb.Max - bb.Min) + bb.Min;
				DrawList->AddLine(p, h1, ImColor(white), LINE_WIDTH / 2);
				DrawList->AddLine(p, h2, ImColor(white), LINE_WIDTH / 2);
				DrawList->AddCircleFilled(h1, GRAB_RADIUS, ImColor(white));
				DrawList->AddCircleFilled(h1, GRAB_RADIUS - GRAB_BORDER, ImColor(blue));
				DrawList->AddCircleFilled(h2, GRAB_RADIUS, ImColor(white));
				DrawList->AddCircleFilled(h2, GRAB_RADIUS - GRAB_BORDER, ImColor(blue));
			}
		}

		return changed;
	}
}
#endif //_BEZIER_CURVE_DRAW_HPP