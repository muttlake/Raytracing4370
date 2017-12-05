#ifndef TRIANGLEH
#define TRIANGLEH

#include "hitable.h"


class triangle: public hitable  {
    public:
        triangle() {}
        triangle(vec3 _v0, vec3 _v1, vec3 _v2, vec3 _n, float _u, float _v, material *mat) : v0(_v0), v1(_v1), v2(_v2), n(_n), tex_u(_u), tex_v(_v), mp(mat) {};
        virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
        vec3 v0, v1, v2;
	vec3 n;
	float tex_u, tex_v;
        material  *mp;
};


bool triangle::hit(const ray& r, float t0, float t1, hit_record& rec) const
{
    const float EPSILON = 0.0000001; 

    vec3 edge1, edge2, h, s, q;
    float a,f,u,v;

    edge1 = v1 - v0;
    edge2 = v2 - v0;

    h = cross(r.direction(), edge2);

    a = dot(edge1, h);
    if (a > -EPSILON && a < EPSILON)
        return false;

    f = 1/a;
    s = r.origin() - v0;

    u = f * dot(s, h);
    if (u < 0.0 || u > 1.0)
        return false;

    q = cross(s, edge1);

    v = f * dot(r.direction(), q);
    if (v < 0.0 || u + v > 1.0)
        return false;

    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * dot(edge2, q);
    if (t > EPSILON) // ray intersection
    {
        rec.p = r.point_at_parameter(t);
        rec.u = u;
        rec.v = v;
        rec.t = t;
        rec.mat_ptr = mp;
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}


#endif


