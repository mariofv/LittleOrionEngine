#ifndef _BEZIER_CURVE_H
#define _BEZIER_CURVE_H

#include <MathGeoLib.h>

class Config;

struct BezierPoint
{
	float2 curve_point;
	float2 left_pivot;
	float2 right_pivot;
};

class BezierCurve
{
public:
	BezierCurve();
	~BezierCurve();

	float2 BezierValue(const float percentage) const;
	void CalculateCurveValues();
	
	bool AddPointAtCurve(const float point_x);
	bool RemovePointWithIndex(const int point_index);

	void MoveHandleToValue(float2& handle, const float2& value, bool calculate_curve = true);
	void MoveHandleByIncrement(float2& handle, float2& increment, bool calculate_curve = true);
	void MovePointToValue(BezierPoint& point, const float2& value);
	void MovePointByIncrement(BezierPoint& point, float2& increment);

	void SpecializedSave(Config& config, const std::string& name) const;
	void SpecializedLoad(const Config& config, const std::string& name);

private:
	float CalculateBezierValue(const float percentage) const;
	float CalculateBezierValueIndexPoints(const float percentage, const int last_index) const;
	void CheckAllPoints();

public:
	static const unsigned int MAXIMUM_POINTS = 10;
	static const unsigned int NUM_CURVE_VALUES = 256;

	BezierPoint points[MAXIMUM_POINTS];
	float values[NUM_CURVE_VALUES + 1];
	int num_points = 2;
};

#endif //_BEZIER_CURVE_H