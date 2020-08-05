#include "BezierCurve.h"



BezierCurve::BezierCurve()
{
	for (int i = 0; i < 10; i++)
	{
		points[i].point = float2(0, 0);
		points[i].left_pivot = points[i].point;
		points[i].right_pivot = float2(0, 0);
	}

	points[0].point = float2(0, 0);
	points[0].left_pivot = float2(0.2, 0.3);
	points[0].right_pivot = float2(0.2, 0.3);

	points[1].point = float2(1, 1);
	points[1].left_pivot = float2(0.8, 0.7);
	points[1].right_pivot = float2(0.8, 0.7);
	
	current_points = 2;
}


BezierCurve::~BezierCurve()
{
}

float2 BezierCurve::BezierValue(const float t) const
{
	assert(t >= 0 && t <= 1);

	//B(t) = (1-t)**3 p0 + 3(1 - t)**2 t P1 + 3(1-t)t**2 P2 + t**3 P3

	float2 val_point = pow((1 - t), 3) * points[0].point + 3 * pow((1 - t), 2) * t * points[0].right_pivot
		+ 3 * (1 - t) * pow(t, 2)* points[1].left_pivot + pow(t, 3) * points[1].point;

	
	//x = (1 - t)*(1 - t)*(1 - t)*p0x + 3 * (1 - t)*(1 - t)*t*p1x + 3 * (1 - t)*t*t*p2x + t * t*t*p3x;
	//y = (1 - t)*(1 - t)*(1 - t)*p0y + 3 * (1 - t)*(1 - t)*t*p1y + 3 * (1 - t)*t*t*p2y + t * t*t*p3y;

	return val_point;
}
