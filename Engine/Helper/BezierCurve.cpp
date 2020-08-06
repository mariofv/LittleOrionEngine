#include "BezierCurve.h"



BezierCurve::BezierCurve()
{
	points[0].curve_point = float2(0, 0);
	points[0].left_pivot = points[0].curve_point;
	points[0].right_pivot = float2(0.2, 0.3);

	points[1].curve_point = float2(1, 1);
	points[1].left_pivot = float2(0.8, 0.7);
	points[1].right_pivot = points[1].curve_point;
	
	current_points = 2;
}


BezierCurve::~BezierCurve()
{
}

float2 BezierCurve::BezierValue(const float t) const
{
	assert(t >= 0 && t <= 1);

	if (current_points == 2)
	{
		return BezierValueIndexPoints(t, 1);
	}

	for (int i = 1; i < MAXIMUM_POINTS; i++)
	{
		if (points[i].curve_point.x >= t)
		{
			float mapped_val = (t - points[i - 1].curve_point.x) / (points[i].curve_point.x - points[i - 1].curve_point.x);
			return BezierValueIndexPoints(mapped_val, i);
		}
	}
	return float2::zero;
}


float2 BezierCurve::BezierValueIndexPoints(const float t, const int i) const
{
	return pow((1 - t), 3) * points[i - 1].curve_point + 3 * pow((1 - t), 2) * t * points[i - 1].right_pivot
		+ 3 * (1 - t) * pow(t, 2)* points[i].left_pivot + pow(t, 3) * points[i].curve_point;
}

bool BezierCurve::AddPoint()
{
	if (current_points >= MAXIMUM_POINTS)
		return false;

	int last_point = current_points - 1;

	points[current_points].curve_point = points[last_point].curve_point;
	points[current_points].left_pivot = points[last_point].left_pivot;
	points[current_points].right_pivot = points[last_point].right_pivot;

	points[last_point].curve_point = (points[last_point].curve_point - points[last_point - 1].curve_point) / 2 + points[last_point - 1].curve_point;
	points[last_point].left_pivot = points[last_point].curve_point - float2(0.2f, 0.0f);
	points[last_point].right_pivot = points[last_point].curve_point + float2(0.2f, 0.0f);
	
	current_points++;
	CheckPointsAndPivots(last_point);

	return true;
}

bool BezierCurve::RemovePoint()
{
	if (current_points <= 2)
		return false;

	int last_point = current_points - 1;

	points[last_point - 1].curve_point = points[last_point].curve_point;
	points[last_point - 1].left_pivot = points[last_point].left_pivot;
	points[last_point - 1].right_pivot = points[last_point].right_pivot;

	current_points--;
	//CheckPointsAndPivots(current_points - 1);

	return true;
}

void BezierCurve::CheckPointsAndPivots(const int index)
{
	if (index == 0)
	{
		points[index].left_pivot.x = 0;
		points[index].curve_point.x = 0;
		points[index].right_pivot.x = points[index].right_pivot.x < 0 ? 0 : (points[index].right_pivot.x > 1 ? 1 : points[index].right_pivot.x);
	}

	else if (index == current_points - 1)
	{
		points[index].left_pivot.x = points[index].left_pivot.x < 0 ? 0 : (points[index].left_pivot.x > 1 ? 1 : points[index].left_pivot.x);
		points[index].curve_point.x = 1;
		points[index].right_pivot.x = 1;
	}

	else
	{
		points[index].left_pivot.x = points[index].left_pivot.x < 0 ? 0 : (points[index].left_pivot.x > 1 ? 1 : points[index].left_pivot.x);
		points[index].curve_point.x = points[index].curve_point.x < points[index - 1].curve_point.x ? points[index - 1].curve_point.x + 0.01f
			: (points[index].curve_point.x > points[index + 1].curve_point.x ? points[index + 1].curve_point.x - 0.01f : points[index].curve_point.x);
		points[index].right_pivot.x = points[index].right_pivot.x < 0 ? 0 : (points[index].right_pivot.x > 1 ? 1 : points[index].right_pivot.x);
	}
}

void BezierCurve::CheckAllPointsAndPivots()
{
	for (int i = 0; i < current_points; i++)
	{
		CheckPointsAndPivots(i);
	}
}
