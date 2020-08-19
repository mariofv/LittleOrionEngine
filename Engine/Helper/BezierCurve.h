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
	
	bool AddPointAtCurve(const float point_x);
	bool RemovePointWithIndex(const int point_index);

	void MovePivotByIncrement(float2& handle, float2& increment);
	void MovePointByIncrement(BezierPoint& point, float2& increment);

	void SpecializedSave(Config& config, const std::string& name) const;
	void SpecializedLoad(const Config& config, const std::string& name);

private:
	float2 BezierValueIndexPoints(const float percentage, const int last_index) const;
	void CheckAllPoints();

public:
	static const unsigned int MAXIMUM_POINTS = 10;

	BezierPoint points[MAXIMUM_POINTS];
	int num_points = 2;
};

#endif //_BEZIER_CURVE_H