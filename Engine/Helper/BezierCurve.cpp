#include "BezierCurve.h"

BezierCurve::BezierCurve()
{
	points[0].curve_point = float2(0, 0);
	points[0].left_pivot = points[0].curve_point;
	points[0].right_pivot = float2(0.2, 0.3);

	points[1].curve_point = float2(1, 1);
	points[1].left_pivot = float2(0.8, 0.7);
	points[1].right_pivot = points[1].curve_point;
	
	num_points = 2;
}


BezierCurve::~BezierCurve()
{
}

float2 BezierCurve::BezierValue(const float percentage) const
{
	assert(percentage >= 0 && percentage <= 1);

	if (num_points == 2)
	{
		return BezierValueIndexPoints(percentage, 1);
	}

	for (int i = 1; i < MAXIMUM_POINTS; i++)
	{
		if (points[i].curve_point.x >= percentage)
		{
			float mapped_val = (percentage - points[i - 1].curve_point.x) / (points[i].curve_point.x - points[i - 1].curve_point.x);
			return BezierValueIndexPoints(mapped_val, i);
		}
	}
	return float2::zero;
}


float2 BezierCurve::BezierValueIndexPoints(const float percentage, const int last_index) const
{
	return pow((1 - percentage), 3) * points[last_index - 1].curve_point + 3 * pow((1 - percentage), 2) * percentage * points[last_index - 1].right_pivot
		+ 3 * (1 - percentage) * pow(percentage, 2)* points[last_index].left_pivot + pow(percentage, 3) * points[last_index].curve_point;
}

bool BezierCurve::AddPoint()
{
	if (num_points >= MAXIMUM_POINTS)
		return false;

	int last_point = num_points - 1;

	points[num_points].curve_point = points[last_point].curve_point;
	points[num_points].left_pivot = points[last_point].left_pivot;
	points[num_points].right_pivot = points[last_point].right_pivot;

	points[last_point].curve_point = (points[last_point].curve_point - points[last_point - 1].curve_point) / 2 + points[last_point - 1].curve_point;
	points[last_point].left_pivot = points[last_point].curve_point - float2(0.2f, 0.0f);
	points[last_point].right_pivot = points[last_point].curve_point + float2(0.2f, 0.0f);
	
	num_points++;
	CheckPointsAndPivots(last_point);

	return true;
}

bool BezierCurve::RemovePoint()
{
	if (num_points <= 2)
		return false;

	int last_point = num_points - 1;

	points[last_point - 1].curve_point = points[last_point].curve_point;
	points[last_point - 1].left_pivot = points[last_point].left_pivot;
	points[last_point - 1].right_pivot = points[last_point].right_pivot;

	num_points--;
	//CheckPointsAndPivots(current_points - 1);

	return true;
}

void BezierCurve::CheckPointsAndPivots(const int index)
{
	points[index].left_pivot.x = points[index].left_pivot.x < 0 ? 0 : (points[index].left_pivot.x > 1 ? 1 : points[index].left_pivot.x);
	points[index].right_pivot.x = points[index].right_pivot.x < 0 ? 0 : (points[index].right_pivot.x > 1 ? 1 : points[index].right_pivot.x);

	if (index == 0)
	{
		points[index].left_pivot.x = 0;
		points[index].curve_point.x = 0;
	}
	else if (index == num_points - 1)
	{
		points[index].curve_point.x = 1;
		points[index].right_pivot.x = 1;
	}
	else
	{
		points[index].curve_point.x = points[index].curve_point.x < points[index - 1].curve_point.x ? points[index - 1].curve_point.x + 0.01f
			: (points[index].curve_point.x > points[index + 1].curve_point.x ? points[index + 1].curve_point.x - 0.01f : points[index].curve_point.x);
	}
}

void BezierCurve::CheckAllPointsAndPivots()
{
	for (int i = 0; i < num_points; i++)
	{
		CheckPointsAndPivots(i);
	}
}
