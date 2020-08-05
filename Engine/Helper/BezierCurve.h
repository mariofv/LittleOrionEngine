#ifndef _BEZIER_CURVE_H
#define _BEZIER_CURVE_H

#include <MathGeoLib.h>

struct BezierPoint
{
	float2 curve_point;
	float2 left_pivot;
	float2 right_pivot;
};

class BezierCurve
{
private:
	static const unsigned int MAXIMUM_POINTS = 10;

public:
	BezierCurve();
	~BezierCurve();

	float2 BezierValue(const float percentage) const;
	
	bool AddPoint();
	bool RemovePoint();

	void CheckPointsAndPivots(const int index);
	void CheckAllPointsAndPivots();

public:
	BezierPoint points[MAXIMUM_POINTS];
	int current_points = 2;
};

#endif //_BEZIER_CURVE_H