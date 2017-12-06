#include <iostream>
#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"
#include "material.h"
#include "texture.h"
#include "ObjHandler.h"
#include "OBJ-Loader/Source/OBJ_Loader.h"
#include <glm_dir/glm/glm.hpp>
#include <glm_dir/glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm_dir/glm/gtx/transform.hpp>

int NUM_TEAPOT_VERTICES = 18960;
int NUM_TEAPOT_TRIANGLES = 6320;

material* loadBrickMaterial()
{
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("/Users/timothymshepard/Cosc4370/RayTracer/Book_Clone/bricks.png", &nx, &ny, &nn, 0);
	material *mat = new lambertian(new image_texture(tex_data, nx, ny));
	return mat;
}

texture* loadSphereCheckerTexture()
{
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("/Users/timothymshepard/Cosc4370/RayTracer/Book_Clone/checker.png", &nx, &ny, &nn, 0);
	return new image_texture(tex_data, nx, ny);
}

vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, MAXFLOAT, rec))
    {
        ray scattered;
        vec3 attenuation;
	if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
	{
       		return attenuation*color(scattered, world, depth+1);
             	//return attenuation;
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
        //return vec3(1.0, 1.0, 1.0);
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

triangle* loadTeapotObj() {
	objl::Loader loader;
	loader.LoadFile("/Users/timothymshepard/Cosc4370/RayTracer/Book_Clone/teapot2.obj");
	ObjHandler handler;
	handler.printObjContents(loader);
        
        int numIndices = loader.LoadedMeshes[0].Indices.size();
        uint* vindices;
        vindices = handler.buildIndicesListFromObj(loader);

	int numVertices = loader.LoadedMeshes[0].Vertices.size();
        glm::vec3* vpositions;
        vpositions = handler.buildPositionsVec3sFromObj(loader);

	glm::vec3* vnormals;
        vnormals = handler.buildNormalsVec3sFromObj(loader);

        glm::vec2* vtex_coords;
        vtex_coords = handler.buildTexCoordVec2sFromObj(loader);

	std::string outFileName = "contentsObj.txt";
	std::ofstream outFile;
    	outFile.open(outFileName);
	
	outFile << "Num Indices: " << numIndices << std::endl;
	outFile << "Num Vertices: " << numVertices << std::endl;

	handler.printAllIndices(vindices, numIndices, outFile);
	handler.printAllPositions(vpositions, numVertices, outFile);
	handler.printAllNormals(vnormals, numVertices, outFile);
	handler.printAllTexCoords(vtex_coords, numVertices, outFile);

    	outFile.close();

	glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f));
	glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(-50.0f), glm::vec3(0,0,1));
	glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, -5.0f, 0.0f));
	glm::mat4 M = T * R * S;

	glm::mat3 N = glm::mat3(M);
	N = glm::transpose(glm::inverse(N));
	
	for (int v_index = 0; v_index < NUM_TEAPOT_VERTICES; v_index++)
	{
		glm::vec3 v = vpositions[v_index];
		glm::vec4 transformed_v = glm::vec4(v[0], v[1], v[2], 1.0f);
		transformed_v = M * transformed_v;
		vpositions[v_index] = glm::vec3(transformed_v[0], transformed_v[1], transformed_v[2]);

		glm::vec3 n = vnormals[v_index];
		glm::vec3 transformed_n = N * n;
		vnormals[v_index] = glm::normalize(transformed_n);
	}

	outFileName = "contentsObj_World.txt";
	outFile.open(outFileName);

	outFile << "Num Indices: " << numIndices << std::endl;
	outFile << "Num Vertices: " << numVertices << std::endl;

	handler.printAllIndices(vindices, numIndices, outFile);
	handler.printAllPositions(vpositions, numVertices, outFile);
	handler.printAllNormals(vnormals, numVertices, outFile);
	handler.printAllTexCoords(vtex_coords, numVertices, outFile);

    	outFile.close();

	outFileName = "TrianglesMade.txt";
	outFile.open(outFileName);

	triangle *all_triangles;
	all_triangles = new triangle[NUM_TEAPOT_TRIANGLES];
	int t_index = 0;
	outFile << "NUM_TEAPOT_TRIANGLES: " << NUM_TEAPOT_TRIANGLES << std::endl;
	outFile << "NUM_TEAPOT_VERTICES: " << NUM_TEAPOT_VERTICES << std::endl;

	vec3 vert0, vert1, vert2;
	vec3 n0, n1, n2;
	glm::vec2 tex0, tex1, tex2;
	material* mp = loadBrickMaterial();

	for (int v_index = 0; v_index < NUM_TEAPOT_VERTICES; v_index++)
	{
		// Some of Teapot vt go to 2 so need to Wrap them around	
		float vtu = vtex_coords[v_index][0];
		if (vtu > 1) vtu = vtu - 1;

		float vtv = vtex_coords[v_index][1];
		if (vtv > 1) vtv = vtv - 1;

		if (((v_index+1) % 3) == 1)
		{
			vert0 = vec3(vpositions[v_index][0], vpositions[v_index][1], vpositions[v_index][2]);
			n0 = vec3(vnormals[v_index][0], vnormals[v_index][1], vnormals[v_index][2]);
			//tex0 = glm::vec2(vtex_coords[v_index][0], vtex_coords[v_index][1]);
			tex0 = glm::vec2(vtu, vtv);
		}
		else if (((v_index+1) % 3) == 2)
		{
			vert1 = vec3(vpositions[v_index][0], vpositions[v_index][1], vpositions[v_index][2]);
			n1 = vec3(vnormals[v_index][0], vnormals[v_index][1], vnormals[v_index][2]);
			//tex1 = glm::vec2(vtex_coords[v_index][0], vtex_coords[v_index][1]);
			tex1 = glm::vec2(vtu, vtv);
		}
		else // (((v_index+1) % 3) == 0)
		{
			vert2 = vec3(vpositions[v_index][0], vpositions[v_index][1], vpositions[v_index][2]);
			n2 = vec3(vnormals[v_index][0], vnormals[v_index][1], vnormals[v_index][2]);
			//tex2 = glm::vec2(vtex_coords[v_index][0], vtex_coords[v_index][1]);
			tex2 = glm::vec2(vtu, vtv);
			
			vec3 normal = (n0 + n1 + n2) / 3;
			
			outFile << "Triangle : " << t_index << std::endl;
			outFile << "	vert0: " << vert0[0] << " , " << vert0[1] << " , " << vert0[2] << std::endl;
			outFile << "	vert1: " << vert1[0] << " , " << vert1[1] << " , " << vert1[2] << std::endl;
			outFile << "	vert2: " << vert2[0] << " , " << vert2[1] << " , " << vert2[2] << std::endl;
			outFile << "	norm0: " << n0[0] << " , " << n0[1] << " , " << n0[2] << std::endl;
			outFile << "	norm1: " << n1[0] << " , " << n1[1] << " , " << n1[2] << std::endl;
			outFile << "	norm2: " << n2[0] << " , " << n2[1] << " , " << n2[2] << std::endl;
			outFile << "	tex0: " << tex0[0] << " , " << tex0[1] << std::endl;
			outFile << "	tex1: " << tex1[0] << " , " << tex1[1] << std::endl;
			outFile << "	tex2: " << tex2[0] << " , " << tex2[1] << std::endl;
			
			all_triangles[t_index] = triangle(vert0, vert1, vert2, n0, n1, n2, tex0, tex1, tex2, mp);
			
			t_index++;
		}
	}

	outFile.close();
	
	delete[] vindices;
	delete[] vpositions;
	delete[] vnormals;
	delete[] vtex_coords;
	
	return all_triangles;
}




hitable *test_scene() {
    int n = 1;

    hitable **list = new hitable*[n+1];
    list[0] = new triangle(vec3(-10, 0, 0), vec3(0, 0, 10), vec3(10, 0, 0), vec3(-0.167119, 0.729682, 0.66305), vec3(-0.0420223,0.611283,0.790296), vec3(-0.0513106,0.746547,0.663351), glm::vec2(0.70, 0.92), glm::vec2(0.6, 0.93), glm::vec2(0.6, 0.92), loadBrickMaterial());
    //list[0] = new triangle(vec3(7.13122, 7.98386, 98.52), vec3(16.2455, 7.65937, 99.3075), vec3(16.1918, 8.48378, 98.52), vec3(-0.167119, 0.729682, 0.66305), vec3(-0.0420223,0.611283,0.790296), vec3(-0.0513106,0.746547,0.663351), glm::vec2(0.70, 0.92), glm::vec2(0.6, 0.93), glm::vec2(0.6, 0.92), loadBrickMaterial());

    return new hitable_list(list,n);
}


hitable *spheres_only_project_scene() {
    int n = 4;
    texture *red_checker = new checker_texture( new constant_texture( vec3(0.2, 0.1, 0.1)), new constant_texture( vec3(0.7, 0.1, 0.1)), 0.50);
    texture *plane_checker = new plane_checker_texture( new constant_texture( vec3(0.5, 0.5, 0.5)), new constant_texture( vec3(0.1, 0.1, 0.1)), -30.0);

    hitable **list = new hitable*[n+1];
    list[0] = new xy_plane(0.0, new texture_metal(plane_checker, 0.0));
    list[1] = new sphere(vec3(15, 2, 6), 6.0, new texture_metal(red_checker, 0.0));
    list[2] = new sphere(vec3(2, -5, 5.5), 5.5, loadBrickMaterial());
    list[3] = new sphere(vec3(-8, -16, 5), 5.0, new dielectric(1.5));
    return new hitable_list(list,n);
}

hitable *first_triangle_project_scene() {
    int n = 4;
    texture *red_checker = new checker_texture( new constant_texture( vec3(0.2, 0.1, 0.1)), new constant_texture( vec3(0.7, 0.1, 0.1)), 0.50);
    texture *plane_checker = new plane_checker_texture( new constant_texture( vec3(0.5, 0.5, 0.5)), new constant_texture( vec3(0.1, 0.1, 0.1)), -30.0);

    hitable **list = new hitable*[n+1];
    list[0] = new xy_plane(0.0, new texture_metal(plane_checker, 0.0));
    list[1] = new sphere(vec3(15, 2, 6), 6.0, new texture_metal(red_checker, 0.0));
    list[2] = new triangle(vec3(-5, 0, 0), vec3(0, 0, 5), vec3(5, 0, 0), vec3(0,0,1), vec3(0,0,1), vec3(0,0,1), glm::vec2(0.5, 0.99), glm::vec2(0.4, 0.98), glm::vec2(0.5, 0.98), loadBrickMaterial());
    list[3] = new sphere(vec3(-8, -16, 5), 5.0, new dielectric(1.5));

    return new hitable_list(list,n);
}

hitable *project_scene() {

    //Loading Triangles for Teapot
    triangle* all_teapot_triangles = loadTeapotObj();
    //NUM_TEAPOT_TRIANGLES = 6320;

    texture *red_checker = new checker_texture( new constant_texture( vec3(0.2, 0.1, 0.1)), new constant_texture( vec3(0.7, 0.1, 0.1)), 0.50);
    texture *plane_checker = new plane_checker_texture( new constant_texture( vec3(0.5, 0.5, 0.5)), new constant_texture( vec3(0.1, 0.1, 0.1)), -30.0);

    int n = 6323;
    hitable **list = new hitable*[n];
    list[0] = new xy_plane(0.0, new texture_metal(plane_checker, 0.0));
    list[1] = new sphere(vec3(15, 2, 6), 6.0, new texture_metal(red_checker, 0.0));
    list[2] = new sphere(vec3(-8, -16, 5), 5.0, new dielectric(1.5));
    
    int teapotIncrement = int(NUM_TEAPOT_TRIANGLES / (n - 3));
    std::cout << "teapot Increment: " << teapotIncrement << std::endl;
    int current_teapot_index = 0;
    for (int ii = 3; ii < n; ii++)
    {
	
	list[ii] = &all_teapot_triangles[current_teapot_index];
	current_teapot_index += teapotIncrement;
    }
    

    delete[] all_teapot_triangles;

    return new hitable_list(list,n);
}

int main(int argc, char *argv[]) {
    if(argc != 2)
    {
	std::cout << "usage: " << argv[0] << " <ppm_file_name>\n";
	return 1;
    }
    
    std::string outfilename = argv[1];

    std::ofstream outFile;
    outFile.open(outfilename);

    int nx = 800;
    int ny = 600;
    int ns = 10;


    outFile << "P3\n" << nx << " " << ny << "\n255\n";
    float R = cos(M_PI/4);
    hitable *world;
    //world = test_scene();
    //world = spheres_only_project_scene();
    //world = first_triangle_project_scene();
    world = project_scene();

    vec3 lookfrom2(0, -70, 15);
    vec3 lookat2(2, 0, 3);
    vec3 up2(0, 0, 1);
    float fov2 = 30;

    camera cam(lookfrom2, lookat2, up2, fov2, float(nx)/float(ny));

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
            outFile << ir << " " << ig << " " << ib << "\n";
        }
    }

    outFile.close();

    return 0;
}


