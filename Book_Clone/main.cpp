#include <iostream>
#include "sphere.h"
#include "plane.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"
#include "material.h"
#include "texture.h"


vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, MAXFLOAT, rec))
    {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
	{
             return attenuation*color(scattered, world, depth+1);
        }
        else
	{
            return vec3(0,0,0);
        }
    }
    else
    {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}


hitable *random_scene() {
    int n = 500;
    hitable **list = new hitable*[n+1];
    list[0] =  new sphere(vec3(0,-1000,0), 1000, new lambertian(new constant_texture(vec3(0.5, 0.5, 0.5))));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = drand48();
            vec3 center(a+0.9*drand48(),0.2,b+0.9*drand48()); 
            if ((center-vec3(4,0.2,0)).length() > 0.9) { 
                if (choose_mat < 0.8) {  // diffuse
                    list[i++] = new sphere(center, 0.2, new lambertian(new constant_texture(vec3(drand48()*drand48(), drand48()*drand48(), drand48()*drand48()))));
                }
                else if (choose_mat < 0.95) { // metal
                    list[i++] = new sphere(center, 0.2,
                            new metal(vec3(0.5*(1 + drand48()), 0.5*(1 + drand48()), 0.5*(1 + drand48())),  0.5*drand48()));
                }
                else {  // glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

    return new hitable_list(list,i);
}

hitable *project_scene() {
    int n = 4;

    texture *checker = new checker_texture( new constant_texture( vec3(0.1, 0.1, 0.1)), new constant_texture( vec3(0.9, 0.9, 0.9)));

    float plane_angle = 30*M_PI/180;
    hitable **list = new hitable*[n+1];
    //list[0] = new plane(vec3(0, 0, 1), 1.0, new lambertian(checker));
    list[0] = new sphere(vec3(0,0,-10000), 10000, new lambertian(checker));
    list[1] = new sphere(vec3(15, 2, 6), 6.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));
    list[2] = new sphere(vec3(-8, -16, 5), 5.0, new dielectric(1.5));
    list[3] = new sphere(vec3(2, -5, 5.5), 5.5, new lambertian(new constant_texture(vec3(1.0, 1.0, 1.0))));

    //hitable **test = new hitable*[1];
    //test[0] = new plane(vec3(0,0,1), 100.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

    return new hitable_list(list,n);
}

int main() {
    int nx = 800;
    int ny = 600;
    int ns = 10;
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    float R = cos(M_PI/4);
    hitable *world;
    world = project_scene();

    vec3 lookfrom2(0, -70, 15);
    vec3 lookat2(2, 0, 3);
    vec3 up2(0, 0, 1);
    float fov2 = 30;

    camera cam(lookfrom2, lookat2, vec3(0,0,1), fov2, float(nx)/float(ny));

    for (int j = ny-1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for (int s=0; s < ns; s++) {
                float u = float(i + drand48()) / float(nx);
                float v = float(j + drand48()) / float(ny);
                ray r = cam.get_ray_simple(u, v);
                vec3 p = r.point_at_parameter(2.0);
                col += color(r, world,0);
            }
            col /= float(ns);
            col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
            int ir = int(255.99*col[0]); 
            int ig = int(255.99*col[1]); 
            int ib = int(255.99*col[2]); 
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}



