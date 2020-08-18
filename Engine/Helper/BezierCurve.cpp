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
	//assert(percentage >= 0 && percentage <= 1);
	if (percentage < 0 || percentage > 1)
		return float2::zero;

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
		assert(i + 1 < MAXIMUM_POINTS);
		assert(i >= 0);
		points[i + 1].left_pivot = points[i].left_pivot;
		points[i + 1].curve_point = points[i].curve_point;
		points[i + 1].right_pivot = points[i].right_pivot;
	}

	points[new_point_index].curve_point = new_point;
	points[new_point_index].left_pivot = points[new_point_index].curve_point - float2(0.05f, 0.0f);
	points[new_point_index].right_pivot = points[new_point_index].curve_point + float2(0.05f, 0.0f);

	num_points++;

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
		assert(i + 1 < MAXIMUM_POINTS);
		assert(i >= 0);
		points[i].left_pivot = points[i + 1].left_pivot;
		points[i].curve_point = points[i + 1].curve_point;
		points[i].right_pivot = points[i + 1].right_pivot;
	}

	num_points--;

	return true;
}

void BezierCurve::MovePivotByIncrement(float2& handle, float2& increment)
{
	float2 change = handle;

	handle += increment;
	handle.x = handle.x < 0 ? 0 : (handle.x > 1 ? 1 : handle.x);
	handle.y = handle.y < 0 ? 0 : (handle.y > 1 ? 1 : handle.y);

	change = handle - change;
	increment = change;
}

void BezierCurve::MovePointByIncrement(BezierPoint& point, float2& increment)
{
	float2 change = point.curve_point;

	point.curve_point += increment;
	CheckAllPoints();

	change = point.curve_point - change;
	
	MovePivotByIncrement(point.left_pivot, change);
	MovePivotByIncrement(point.right_pivot, change);
	increment = change;
}

void BezierCurve::CheckAllPoints()
{
	for (int i = 0; i < num_points; i++)
	{
		if (i == 0)
			points[i].curve_point.x = 0;

		else if (i == num_points - 1)
			points[i].curve_point.x = 1;
			
		else
			points[i].curve_point.x = points[i].curve_point.x < points[i - 1].curve_point.x ? points[i - 1].curve_point.x + 0.01f
				: (points[i].curve_point.x > points[i + 1].curve_point.x ? points[i + 1].curve_point.x - 0.01f : points[i].curve_point.x);
	}
}
