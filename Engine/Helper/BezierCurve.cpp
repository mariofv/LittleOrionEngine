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

	for (int i = 1; i < num_points; i++)
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
	assert(percentage >= 0 && percentage <= 1);

	return pow((1 - percentage), 3) * points[last_index - 1].curve_point + 3 * pow((1 - percentage), 2) * percentage * points[last_index - 1].right_pivot
		+ 3 * (1 - percentage) * pow(percentage, 2)* points[last_index].left_pivot + pow(percentage, 3) * points[last_index].curve_point;
}

bool BezierCurve::AddPointAtCurve(const float point_x)
{
	if (num_points >= MAXIMUM_POINTS)
		return false;

	if (point_x <= 0 || point_x >= 1)
		return false;

	int new_point_index = 0;
	for (int i = 0; i < num_points; i++)
	{
		if (point_x > points[i].curve_point.x)
		{
			new_point_index = i + 1;
			continue;
		}
	}

	float2 new_point = BezierValue(point_x);
	for (int i = num_points - 1; i >= new_point_index; i--)
	{
		points[i + 1].left_pivot = points[i].left_pivot;
		points[i + 1].curve_point = points[i].curve_point;
		points[i + 1].right_pivot = points[i].right_pivot;
	}

	points[new_point_index].curve_point = new_point;
	points[new_point_index].left_pivot = points[new_point_index].curve_point - float2(0.05f, 0.0f);
	points[new_point_index].right_pivot = points[new_point_index].curve_point + float2(0.05f, 0.0f);

	num_points++;
	CheckPointsAndPivots(new_point_index);

	return true;
}

bool BezierCurve::RemovePointWithIndex(const int point_index)
{
	if (num_points <= 2)
		return false;

	if (point_index <= 0 || point_index >= num_points - 1)
		return false;

	for (int i = point_index; i < num_points; i++)
	{
		points[i].left_pivot = points[i + 1].left_pivot;
		points[i].curve_point = points[i + 1].curve_point;
		points[i].right_pivot = points[i + 1].right_pivot;
	}

	num_points--;

	return true;
}

void BezierCurve::CheckAllPointsAndPivots()
{
	for (int i = 0; i < num_points; i++)
	{
		CheckPointsAndPivots(i);
	}
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
