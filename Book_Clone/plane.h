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

class yz_plane: public hitable  {
    public:
        yz_plane() {}
        yz_plane(float _k, material *mat) : k(_k), mp(mat) {};
        //yz_plane(float _y0, float _y1, float _z0, float _z1, float _k, material *mat) : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {};
        virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
        //virtual bool bounding_box(float t0, float t1, aabb& box) const {
        //       box =  aabb(vec3(x0,y0, k-0.0001), vec3(x1, y1, k+0.0001));
        //       return true; }
        material  *mp;
        //float y0, y1, z0, z1, k;
        float k;
};


bool yz_plane::hit(const ray& r, float t0, float t1, hit_record& rec) const {
    float t = (k-r.origin().x()) / r.direction().x();
    if (t < t0 || t > t1)
        return false;
    float y = r.origin().y() + t*r.direction().y();
    float z = r.origin().z() + t*r.direction().z();
    //if (y < y0 || y > y1 || z < z0 || z > z1) 
    //    return false;
    //rec.u = (y-y0)/(y1-y0);
    //rec.v = (z-z0)/(z1-z0); 
    rec.u = 0.5;
    rec.v = 0.5;
    rec.t = t;
    rec.mat_ptr = mp;
    rec.p = r.point_at_parameter(t);
    rec.normal = vec3(1, 0, 0);
    return true;
}


#endif


