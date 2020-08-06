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
public:
	BezierCurve();
	~BezierCurve();

	float2 BezierValue(const float percentage) const;
	
	bool AddPoint();
	bool RemovePoint();

	void CheckPointsAndPivots(const int index);
	void CheckAllPointsAndPivots();

private:
	float2 BezierValueIndexPoints(const float percentage, const int last_index) const;


public:
	static const unsigned int MAXIMUM_POINTS = 10;

	BezierPoint points[MAXIMUM_POINTS];
	int num_points = 2;
};

#endif //_BEZIER_CURVE_H