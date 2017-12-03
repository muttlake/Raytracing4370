#ifndef PLANEH
#define PLANEH

#include "hitable.h"


class xy_plane: public hitable  {
    public:
        xy_plane() {}
        xy_plane(float _k, material *mat) : k(_k), mp(mat) {};
        virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
        //virtual bool bounding_box(float t0, float t1, aabb& box) const {
        //       box =  aabb(vec3(x0,y0, k-0.0001), vec3(x1, y1, k+0.0001));
        //       return true; }
        material  *mp;
        float k;
};


bool xy_plane::hit(const ray& r, float t0, float t1, hit_record& rec) const {
    float t = (k-r.origin().z()) / r.direction().z();
    if (t < t0 || t > t1)
        return false;
    float x = r.origin().x() + t*r.direction().x();
    float y = r.origin().y() + t*r.direction().y();
    //if (x < x0 || x > x1 || y < y0 || y > y1) 
    //    return false;
    //rec.u = (x-x0)/(x1-x0);
    //rec.v = (y-y0)/(y1-y0); 
    rec.u = 0.5;
    rec.v = 0.5;
    rec.t = t;
    rec.mat_ptr = mp;
    rec.p = r.point_at_parameter(t);
    rec.normal = vec3(0, 0, 1);
    return true;
}


//
//class xy_plane: public hitable  {
//    public:
//        xy_plane() {}
//        xy_plane(vec3 n, float d, material *m) : normal(n), distance(d), mat_ptr(m)  {};
//        virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
//        vec3 normal;
//        float distance;
//        material *mat_ptr;
//};
//
//bool xy_plane::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
//{
//    //ray_direction
//    double a = dot(r.direction(), normal);
//
//    if (a > 1e-6)
//    {
//	vec3 perpPointOnPlane(0, 0, 0);
//	perpPointOnPlane = r.origin() + (-1 * normal * distance);
//	float t = dot(perpPointOnPlane, normal) / a;
//	
//	if (t >= 0)
//	{
//		rec.t = t;
//		rec.p = perpPointOnPlane;
//		rec.normal = normal;
//		rec.mat_ptr = mat_ptr;
//		return true;
//	}
//	
//    }
//    return false;
//}
//
//struct hit_record
//{
//    float t;  
//    vec3 p;
//    vec3 normal; 
//    material *mat_ptr;
//};

#endif


