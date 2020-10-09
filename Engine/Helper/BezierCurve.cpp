#include "BezierCurve.h"

#include "Helper/Config.h"

BezierCurve::BezierCurve()
{
	points[0].curve_point = float2(0, 0);
	points[0].left_pivot = points[0].curve_point;
	points[0].right_pivot = float2(0.2, 0.3);

	points[1].curve_point = float2(1, 1);
	points[1].left_pivot = float2(0.8, 0.7);
	points[1].right_pivot = points[1].curve_point;
	
	num_points = 2;

	CalculateCurveValues();
}


BezierCurve::~BezierCurve()
{
}

float2 BezierCurve::BezierValue(const float percentage) const
{
	int index = percentage * NUM_CURVE_VALUES;
	return float2(percentage, values[index]);
}

void BezierCurve::CalculateCurveValues()
{
	for (int i = 0; i <= NUM_CURVE_VALUES; i++)
	{
		values[i] = CalculateBezierValue((float)i / (float)NUM_CURVE_VALUES);
	}
}

float BezierCurve::CalculateBezierValue(const float percentage) const
{
	if (percentage <= 0)
		return points[0].curve_point.y;

	if (percentage >= 1)
		return points[num_points - 1].curve_point.y;

	if (num_points == 2)
	{
		return CalculateBezierValueIndexPoints(percentage, 1);
	}

	for (int i = 1; i < num_points; i++)
	{
		if (points[i].curve_point.x >= percentage)
		{
			float mapped_val = (percentage - points[i - 1].curve_point.x) / (points[i].curve_point.x - points[i - 1].curve_point.x);
			return CalculateBezierValueIndexPoints(mapped_val, i);
		}
	}
	return 0;
}

float BezierCurve::CalculateBezierValueIndexPoints(const float percentage, const int last_index) const
{
	assert(percentage >= 0 && percentage <= 1);

	float2 point = pow((1 - percentage), 3) * points[last_index - 1].curve_point + 3 * pow((1 - percentage), 2) * percentage * points[last_index - 1].right_pivot
		+ 3 * (1 - percentage) * pow(percentage, 2)* points[last_index].left_pivot + pow(percentage, 3) * points[last_index].curve_point;
	
	return point.y;
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

	CalculateCurveValues();

	return true;
}

bool BezierCurve::RemovePointWithIndex(const int point_index)
{
	if (num_points <= 2)
		return false;

	if (point_index <= 0 || point_index >= num_points - 1)
		return false;

	for (int i = point_index; i < num_points - 1; i++)
	{
		assert(i + 1 < MAXIMUM_POINTS);
		assert(i >= 0);
		points[i].left_pivot = points[i + 1].left_pivot;
		points[i].curve_point = points[i + 1].curve_point;
		points[i].right_pivot = points[i + 1].right_pivot;
	}

	num_points--;

	CalculateCurveValues();

	return true;
}

void BezierCurve::MoveHandleToValue(float2& handle, const float2& value, bool calculate_curve)
{
	handle = Clamp(value, 0.0F, 1.0F);

	if (calculate_curve)
	{
		CalculateCurveValues();
	}
}

void BezierCurve::MoveHandleByIncrement(float2& handle, float2& increment, bool calculate_curve)
{
	float2 change = handle;

	handle += increment;
	handle = Clamp(handle, 0.0F, 1.0F);

	change = handle - change;
	increment = change;

	if (calculate_curve)
	{
		CalculateCurveValues();
	}
}

void BezierCurve::MovePointToValue(BezierPoint& point, const float2& value)
{
	float2 change = point.curve_point;

	point.curve_point = Clamp(value, 0.0F, 1.0F);
	CheckAllPoints();

	change = point.curve_point - change;
	MoveHandleByIncrement(point.left_pivot, change, false);
	MoveHandleByIncrement(point.right_pivot, change, false);

	CalculateCurveValues();
}

void BezierCurve::MovePointByIncrement(BezierPoint& point, float2& increment)
{
	float2 change = point.curve_point;

	point.curve_point += increment;
	CheckAllPoints();

	change = point.curve_point - change;
	
	MoveHandleByIncrement(point.left_pivot, change, false);
	MoveHandleByIncrement(point.right_pivot, change, false);
	increment = change;

	CalculateCurveValues();
}

void BezierCurve::SpecializedSave(Config & config, const std::string& name) const
{
	config.AddInt(num_points, name + " Num Points");

	for (int i = 0; i < num_points; i++)
	{
		config.AddFloat2(points[i].curve_point, name + " Point " + std::to_string(i));
		config.AddFloat2(points[i].left_pivot, name + " Left Pivot " + std::to_string(i));
		config.AddFloat2(points[i].right_pivot, name + " Right Pivot " + std::to_string(i));
	}
}

void BezierCurve::SpecializedLoad(const Config & config, const std::string& name)
{
	num_points = config.GetInt(name + " Num Points", 2);

	for (int i = 0; i < num_points; i++)
	{
		config.GetFloat2(name + " Point " + std::to_string(i), points[i].curve_point, float2::zero);
		config.GetFloat2(name + " Left Pivot " + std::to_string(i), points[i].left_pivot, float2::zero);
		config.GetFloat2(name + " Right Pivot " + std::to_string(i), points[i].right_pivot, float2::zero);
	}

	CheckAllPoints();
	CalculateCurveValues();
}

void BezierCurve::CheckAllPoints()
{
	for (int i = 0; i < num_points; i++)
	{
		if (i == 0)
		{
			points[i].curve_point.x = 0;
		}
		else if (i == num_points - 1)
		{
			points[i].curve_point.x = 1;
		}
		else
		{
			points[i].curve_point.x = points[i].curve_point.x < points[i - 1].curve_point.x ? points[i - 1].curve_point.x + 0.01f
				: (points[i].curve_point.x > points[i + 1].curve_point.x ? points[i + 1].curve_point.x - 0.01f : points[i].curve_point.x);
		}

		points[i].curve_point.y = points[i].curve_point.y < 0 ? 0 : (points[i].curve_point.y > 1 ? 1 : points[i].curve_point.y);
	}
}
