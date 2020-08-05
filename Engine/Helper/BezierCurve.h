#ifndef _BEZIER_CURVE_H
#define _BEZIER_CURVE_H

#include <MathGeoLib.h>

struct BezierPoint
{
	float2 point;
	float2 left_pivot;
	float2 right_pivot;
};

class BezierCurve
{
public:
	BezierCurve();
	~BezierCurve();

	float2 BezierValue(const float percentage) const;

public:
	BezierPoint points[10];
	int current_points = 2;
};

#endif //_BEZIER_CURVE_H