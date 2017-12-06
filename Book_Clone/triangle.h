#ifndef TRIANGLEH
#define TRIANGLEH

#include <glm_dir/glm/glm.hpp>
#include <glm_dir/glm/gtc/type_ptr.hpp>
#include "math.h"

#include "hitable.h"

float distance(vec3 pt1, vec3 pt2)
{
	float sum_of_squares = (pt2[0] - pt1[0]) * (pt2[0] - pt1[0]);
	sum_of_squares += (pt2[1] - pt1[1]) * (pt2[1] - pt1[1]);
	sum_of_squares += (pt2[2] - pt1[2]) * (pt2[2] - pt1[2]);
	
	return sqrt(sum_of_squares);
}

class triangle: public hitable  {
    public:
        triangle() {}
        triangle(vec3 _vv0, vec3 _vv1, vec3 _vv2, vec3 _nn0, vec3 _nn1, vec3 _nn2, glm::vec2 _tt0, glm::vec2 _tt1, glm::vec2 _tt2, material *mat) : v0(_vv0), v1(_vv1), v2(_vv2), n0(_nn0), n1(_nn1), n2(_nn2), tex0(_tt0), tex1(_tt1), tex2(_tt2),  mp(mat) {};
        virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
	vec3 get_triangle_barycentric_coords(vec3& p) const;
        vec3 v0, v1, v2;
	vec3 n0, n1, n2;
	glm::vec2 tex0, tex1, tex2;
        material  *mp;
};

vec3 triangle::get_triangle_barycentric_coords(vec3& p) const
{
	//float dA = distance(v0, p);
	//float dB = distance(v1, p);
	//float dC = distance(v2, p);
	//
	//float sum = dA + dB + dC;

	//float coefA = dA / sum;
	//float coefB = dB / sum;
	//float coefC = dC / sum;
	//
	//return vec3(coefA, coefB, coefC);
	
	vec3 e0 = v1 - v0;
	vec3 e1 = v2 - v0;
	vec3 e2 = p  - v0;
	
	float d00 = dot(e0, e0);
	float d01 = dot(e0, e1);
	float d11 = dot(e1, e1);
	float d20 = dot(e2, e0);
	float d21 = dot(e2, e1);
	
	float denom = d00 * d11 - d01 * d01;
	
	float alph = (d11 * d20 - d01 * d21) / denom;
	float beta = (d00 * d21 - d01 * d20) / denom;
	float gamm = 1.0f - alph - beta;
	
	return vec3(alph, beta, gamm);
}

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
	if (t < t0 || t > t1)
	{
		return false;
	}
	else
	{
	        rec.t = t;
	        rec.p = r.point_at_parameter(t);
		
		vec3 bary = get_triangle_barycentric_coords(rec.p);

	        rec.u = tex0[0]*bary[0] + tex1[0]*bary[1] + tex2[0]*bary[2];
	        rec.v = tex0[1]*bary[0] + tex1[1]*bary[1] + tex2[1]*bary[2];
		rec.normal = n0*bary[0] + n1*bary[1] + n2*bary[2];

	        rec.mat_ptr = mp;
	        return true;
	}
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}


#endif

