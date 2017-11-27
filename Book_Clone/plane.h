#ifndef PLANEH
#define PLANEH

#include "hitable.h"

class plane: public hitable  {
    public:
        plane() {}
        plane(vec3 n, float d, material *m) : normal(n), distance(d), mat_ptr(m)  {};
        virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
        vec3 normal;
        float distance;
        material *mat_ptr;
};

bool plane::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
    //ray_direction
    double a = dot(r.direction(), normal);

    if (a > 1e-6)
    {
	vec3 perpPointOnPlane(0, 0, 0);
	perpPointOnPlane = r.origin() + (-1 * normal * distance);
	float t = dot(perpPointOnPlane, normal) / a;
	
	if (t >= 0)
	{
		rec.t = t;
		rec.p = perpPointOnPlane;
		rec.normal = normal;
		rec.mat_ptr = mat_ptr;
		return true;
	}
	
    }
    return false;
}

//struct hit_record
//{
//    float t;  
//    vec3 p;
//    vec3 normal; 
//    material *mat_ptr;
//};

#endif


