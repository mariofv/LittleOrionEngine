#ifndef _BEZIER_CURVE_DRAW_HPP
#define _BEZIER_CURVE_DRAW_HPP

#define IMGUI_DEFINE_MATH_OPERATORS

#include <Helper/BezierCurve.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace ImGui
{
	static int const SMOOTHNESS = 128;
	static int const CURVE_WIDTH = 3;
	static int const LINE_WIDTH = 1;
	static int const GRAB_RADIUS = 4;
	static int const GRAB_BORDER = 1;
	static int const POINT_RADIUS = 4;
	static int const POINT_VERTEXS = 4;
	static int const AREA_WIDTH = 0;

	static void DrawRangeGuide(ImVec2& range, ImVec2& canvas, ImDrawList* draw_list, ImRect& rec)
	{
		ImVec4 white(GetStyle().Colors[ImGuiCol_Text]);
		float spacing = canvas.y / 4;
		char display[10];

		for (int i = 0; i <= 4; i++)
		{
			float value = (range.y - range.x) / 4 * (4 - i) + range.x;
			sprintf(display, "%.2f", value);
			if (i == 4)
				draw_list->AddText(ImVec2(rec.Min.x + 3, rec.Max.y - 17), ImColor(white), display);
			else
				draw_list->AddText(ImVec2(rec.Min.x + 3, rec.Min.y + spacing * i + 1), ImColor(white), display);
		}
	}
		
	static void DrawGrid(ImVec2& canvas, ImDrawList* draw_list, ImRect& rec)
	{
		for (int i = 0; i <= canvas.x; i += (canvas.x / 4)) {
			draw_list->AddLine(
				ImVec2(rec.Min.x + i, rec.Min.y),
				ImVec2(rec.Min.x + i, rec.Max.y),
				GetColorU32(ImGuiCol_TextDisabled));
		}
		for (int i = 0; i <= canvas.y; i += (canvas.y / 4)) {
			draw_list->AddLine(
				ImVec2(rec.Min.x, rec.Min.y + i),
				ImVec2(rec.Max.x, rec.Min.y + i),
				GetColorU32(ImGuiCol_TextDisabled));
		}
	}

	static void DrawCurve(const float2 (&results)[SMOOTHNESS + 1], ImDrawList* draw_list, ImRect& rec)
	{
		ImColor color(GetStyle().Colors[ImGuiCol_PlotLines]);
		for (int i = 0; i <= SMOOTHNESS - 1; i++)
		{
			ImVec2 p = { results[i + 0].x, 1 - results[i + 0].y };
			ImVec2 q = { results[i + 1].x, 1 - results[i + 1].y };
			ImVec2 r(p.x * (rec.Max.x - rec.Min.x) + rec.Min.x, p.y * (rec.Max.y - rec.Min.y) + rec.Min.y);
			ImVec2 s(q.x * (rec.Max.x - rec.Min.x) + rec.Min.x, q.y * (rec.Max.y - rec.Min.y) + rec.Min.y);
			draw_list->AddLine(r, s, color, CURVE_WIDTH);
		}
	}

	static void DrawPoint(const ImVec2& point, ImDrawList* draw_list)
	{
		ImVec4 white(GetStyle().Colors[ImGuiCol_Text]);
		draw_list->AddNgonFilled(point, POINT_RADIUS, ImColor(white), POINT_VERTEXS);
	}

	static void DrawHandleForPoint(const ImVec2& handle, const ImVec2& point, ImDrawList* draw_list)
	{
		ImVec4 white(GetStyle().Colors[ImGuiCol_Text]);
		ImVec4 blue(0.15f, 0.23f, 0.40f, 0.7f);

		draw_list->AddLine(point, handle, ImColor(white), LINE_WIDTH / 3);
		draw_list->AddCircleFilled(handle, GRAB_RADIUS, ImColor(white));
		draw_list->AddCircleFilled(handle, GRAB_RADIUS - GRAB_BORDER, ImColor(blue));
	}

	static void ControlBezierPointsAndHandles(BezierCurve* bezier, bool& changed, ImVec2& canvas, ImRect& rec)
	{

		ImVec2 mouse = GetIO().MousePos, pos[3 * bezier->MAXIMUM_POINTS];
		float total_points_and_pivots = bezier->num_points * 3 - 2;
		static int current_point_index = -1, current_point_side = 0; //side: 0=center, -1=left, 1=right
		static float current_point_distance = 1000000;

		//check interaction with mouse
		if (!IsMouseDoubleClicked(0) && !IsMouseClicked(0) && !IsMouseReleased(0) && !IsMouseDragging(0))
		{
			return;
		}

		if (IsMouseReleased(0))
		{
			current_point_index = -1;
			return;
		}

		if (IsMouseDoubleClicked(0) || !IsMouseClicked(0))
		{
			//build point and handle array
			for (int i = 0; i < bezier->num_points; i++)
			{
				if (i == 0)
				{
					pos[0] = ImVec2(bezier->points[i].curve_point.x, 1 - bezier->points[i].curve_point.y) * (rec.Max - rec.Min) + rec.Min;
					pos[1] = ImVec2(bezier->points[i].right_pivot.x, 1 - bezier->points[i].right_pivot.y) * (rec.Max - rec.Min) + rec.Min;
				}

				else if (i == bezier->num_points - 1)
				{
					pos[2 + (i - 1) * 3 + 0] = ImVec2(bezier->points[i].left_pivot.x, 1 - bezier->points[i].left_pivot.y) * (rec.Max - rec.Min) + rec.Min;
					pos[2 + (i - 1) * 3 + 1] = ImVec2(bezier->points[i].curve_point.x, 1 - bezier->points[i].curve_point.y) * (rec.Max - rec.Min) + rec.Min;
				}

				else
				{
					pos[2 + (i - 1) * 3 + 0] = ImVec2(bezier->points[i].left_pivot.x, 1 - bezier->points[i].left_pivot.y) * (rec.Max - rec.Min) + rec.Min;
					pos[2 + (i - 1) * 3 + 1] = ImVec2(bezier->points[i].curve_point.x, 1 - bezier->points[i].curve_point.y) * (rec.Max - rec.Min) + rec.Min;
					pos[2 + (i - 1) * 3 + 2] = ImVec2(bezier->points[i].right_pivot.x, 1 - bezier->points[i].right_pivot.y) * (rec.Max - rec.Min) + rec.Min;
				}
			}

			//find closest point or handle
			if (current_point_index == -1)
			{
				current_point_distance = 1000000;
				for (int i = 0; i < total_points_and_pivots; i++)
				{
					float distance = (pos[i].x - mouse.x) * (pos[i].x - mouse.x) + (pos[i].y - mouse.y) * (pos[i].y - mouse.y);
					if (distance < current_point_distance)
					{
						current_point_distance = distance;

						if (i < 2)
						{
							current_point_index = 0;
							current_point_side = i;
						}
						else if (i > total_points_and_pivots - 2)
						{
							current_point_index = bezier->num_points - 1;
							current_point_side = (i + 1) - total_points_and_pivots;
						}
						else
						{
							current_point_index = (i - ((i - 2) % 3)) / 3 + 1;
							current_point_side = (i - 2) % 3 - 1;
						}
					}
				}
			}
		}

		if (current_point_index == -1)
		{
			return;
		}

		//add or remove points
		int detection_distance = 4 * GRAB_RADIUS * 4 * GRAB_RADIUS;
		if (IsMouseDoubleClicked(0) && rec.Contains(mouse))
		{
			if (current_point_distance < detection_distance && current_point_side == 0)
				bezier->RemovePointWithIndex(current_point_index);
			else
				bezier->AddPointAtCurve((mouse.x - rec.Min.x) / (rec.Max.x - rec.Min.x));
		}

		//drag and move points
		else if (current_point_distance < detection_distance)
		{
			if (IsMouseClicked(0) || IsMouseDragging(0) && rec.Contains(mouse))
			{
				if (current_point_side == -1)
					bezier->MoveHandleToValue(bezier->points[current_point_index].left_pivot, float2((GetIO().MousePos.x - rec.Min.x ) / canvas.x, -(GetIO().MousePos.y - rec.Max.y)/ canvas.y));

				else if (current_point_side == 0)
					bezier->MovePointToValue(bezier->points[current_point_index], float2((GetIO().MousePos.x - rec.Min.x) / canvas.x, -(GetIO().MousePos.y - rec.Max.y) / canvas.y));

				else
					bezier->MoveHandleToValue(bezier->points[current_point_index].right_pivot, float2((GetIO().MousePos.x - rec.Min.x) / canvas.x, -(GetIO().MousePos.y - rec.Max.y) / canvas.y));

				changed = true;
			}
		}
	}

	static bool DrawBezierCubic(BezierCurve* bezier, ImVec2& range)
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

		//draw background grid
		DrawGrid(Canvas, DrawList, bb);

		//draw range guide
		DrawRangeGuide(range, Canvas, DrawList, bb);

		// build curve
		float2 results[SMOOTHNESS + 1];
		for (int i = 0; i <= SMOOTHNESS; i++)
		{
			results[i] = bezier->BezierValue(float(i) / (float)SMOOTHNESS);
		}

		// handle and point grabbers
		ControlBezierPointsAndHandles(bezier, changed, Canvas, bb);

		// draw curve
		DrawCurve(results, DrawList, bb);

		//draw curve points and grabbers
		for (int i = 0; i < bezier->num_points; i++)
		{
			ImVec2 p = ImVec2(bezier->points[i].curve_point.x, 1 - bezier->points[i].curve_point.y) * (bb.Max - bb.Min) + bb.Min;
			DrawPoint(p, DrawList);

			if (i == 0)
			{
				ImVec2 h = ImVec2(bezier->points[i].right_pivot.x, 1 - bezier->points[i].right_pivot.y) * (bb.Max - bb.Min) + bb.Min;
				DrawHandleForPoint(h, p, DrawList);
			}

			else if (i == bezier->num_points - 1)
			{
				ImVec2 h = ImVec2(bezier->points[i].left_pivot.x, 1 - bezier->points[i].left_pivot.y) * (bb.Max - bb.Min) + bb.Min;
				DrawHandleForPoint(h, p, DrawList);
			}

			else
			{
				ImVec2 h2 = ImVec2(bezier->points[i].left_pivot.x, 1 - bezier->points[i].left_pivot.y) * (bb.Max - bb.Min) + bb.Min;
				ImVec2 h1 = ImVec2(bezier->points[i].right_pivot.x, 1 - bezier->points[i].right_pivot.y) * (bb.Max - bb.Min) + bb.Min;
				DrawHandleForPoint(h1, p, DrawList);
				DrawHandleForPoint(h2, p, DrawList);
			}
		}

		return changed;
	}
}
#endif //_BEZIER_CURVE_DRAW_HPP