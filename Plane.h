#ifndef	_PLANE_H
#define _PLANE_H

#include "math.h"
#include "Object.h"
#include "Vect.h"
#include "Color.h"

class Plane: public Object
{
	Vect normal;
	double distance;
	Color color;

	public:

	Plane();

	Plane(Vect, double, Color);

	// method functions
	Vect getPlaneNormal() { return normal; }
	double getPlaneDistance() { return distance; }
	virtual Color getColor() { return color; }

	virtual Vect getNormalAt(Vect point)
	{
		return normal;
	}

	// double is distance from ray origin to point of intersection
	virtual double findIntersection(Ray ray)
	{
		Vect ray_direction = ray.getRayDirection();

		double a = ray_direction.dotProduct(normal);

		if (a == 0)
		{
			//Ray is parallel to plane
			return -1;
		}
		else
		{
			double b = normal.dotProduct(ray.getRayOrigin().vectAdd(normal.vectMult(distance).negative()));
			return -1 * b / a;
		}
	}

};

Plane::Plane()
{
	normal = Vect( 1, 0, 0);
	distance = 1.0;
	color = Color( 0.5, 0.5, 0.5, 0);
}

Plane::Plane(Vect normalValue, double distanceValue, Color colorValue)
{
	normal = normalValue;
	distance = distanceValue;
	color = colorValue;
}

#endif
