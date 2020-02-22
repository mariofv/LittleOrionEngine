#ifndef _POINT_H_
#define _POINT_H_

template <typename T>
class Point
{
public:
	T x, y;

	Point() {}
	Point(T x, T y) : x(x), y(y) {}

	// Operators ------------------------------------------------
	Point operator -(const Point &v) const
	{
		Point r;
		r.x = x - v.x;
		r.y = y - v.y;
		return(r);
	}

	Point operator + (const Point &v) const
	{
		Point r;
		r.x = x + v.x;
		r.y = y + v.y;
		return(r);
	}

	const Point& operator -=(const Point &v)
	{
		x -= v.x;
		y -= v.y;
		return(*this);
	}

	const Point& operator +=(const Point &v)
	{
		x += v.x;
		y += v.y;
		return(*this);
	}

	bool operator ==(const Point& v) const
	{
		return (x == v.x && y == v.y);
	}

	bool operator !=(const Point& v) const
	{
		return (x != v.x || y != v.y);
	}

	// Utils ------------------------------------------------
	bool IsZero() const
	{
		return (x == 0 && y == 0);
	}

	Point& SetToZero()
	{
		x = y = 0;
		return(*this);
	}

	Point& Negate()
	{
		x = -x;
		y = -y;
		return(*this);
	}

	// Distances ---------------------------------------------
	T DistanceTo(const Point& v) const
	{
		T fx = x - v.x;
		T fy = y - v.y;
		return sqrt((fx*fx) + (fy*fy));
	}
};

typedef Point<int32_t> iPoint;
typedef Point<float> fPoint;
typedef Point<double> dPoint;

#endif // _POINT_H_