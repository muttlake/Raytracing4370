#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "Vect.h"
#include "Ray.h"
#include "Camera.h"
#include "Color.h"
#include "Source.h"
#include "Light.h"
#include "Object.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"

#include "ObjLoader.h"

using namespace std;

struct RGBType 
{
	double r;
	double g;
	double b;
};

//Makes file on hard drive
void savebmp(const char *filename, int w, int h, int dpi, RGBType *data)
{
	FILE *f;
	int k = w*h;
	int s = 4*k;
	int filesize = 54 + s;

	double factor = 39.375;
	int m = static_cast<int>(factor);

	int ppm = dpi*m;

	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0,0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0};

	bmpfileheader[ 2] = (unsigned char)(filesize);
	bmpfileheader[ 3] = (unsigned char)(filesize>>8);
	bmpfileheader[ 4] = (unsigned char)(filesize>>16);
	bmpfileheader[ 5] = (unsigned char)(filesize>>24);

	bmpinfoheader[ 4] = (unsigned char)(w);
	bmpinfoheader[ 5] = (unsigned char)(w>>8);
	bmpinfoheader[ 6] = (unsigned char)(w>>16);
	bmpinfoheader[ 7] = (unsigned char)(w>>24);

	bmpinfoheader[ 8] = (unsigned char)(h);
	bmpinfoheader[ 9] = (unsigned char)(h>>8);
	bmpinfoheader[10] = (unsigned char)(h>>16);
	bmpinfoheader[11] = (unsigned char)(h>>24);

	bmpinfoheader[21] = (unsigned char)(s);
	bmpinfoheader[22] = (unsigned char)(s>>8);
	bmpinfoheader[23] = (unsigned char)(s>>16);
	bmpinfoheader[24] = (unsigned char)(s>>24);

	bmpinfoheader[25] = (unsigned char)(ppm);
	bmpinfoheader[26] = (unsigned char)(ppm>>8);
	bmpinfoheader[27] = (unsigned char)(ppm>>16);
	bmpinfoheader[28] = (unsigned char)(ppm>>24);

	bmpinfoheader[29] = (unsigned char)(ppm);
	bmpinfoheader[30] = (unsigned char)(ppm>>8);
	bmpinfoheader[31] = (unsigned char)(ppm>>16);
	bmpinfoheader[32] = (unsigned char)(ppm>>24);

	f = fopen(filename,"wb");

	fwrite(bmpfileheader, 1, 14, f);

	fwrite(bmpinfoheader, 1, 40, f);

	for (int i = 0; i < k; i++)
	{
		RGBType rgb = data[i];
		double red = (data[i].r)*255;
		double green = (data[i].g)*255;
		double blue = (data[i].b)*255;

		unsigned char color[3] = {(int)floor(blue), (int)floor(green), (int)floor(red)};

		fwrite(color, 1, 3, f);
	}

	fclose(f);
}


int winningObjectIndex(vector<double> object_intersections)
{
	// return the index of the winning intersection
	int index_of_minimum_value;

	// prevent unnecessary calculations
	if (object_intersections.size() == 0)
	{
		// there are no intersections
		return -1;
	}
	else if (object_intersections.size() == 1)
	{
		//if the intersection is greater than zero then its our index of minimum value
		if (object_intersections.at(0) > 0) 
		{
			return 0; // winning object index
		}
		else // the only intersection is negative
		{
			return -1;
		}
	}
	else // more than one object_intersections
	{
		double max = 0;
		for (int i = 0; i < object_intersections.size(); i++)
		{
			if (object_intersections.at(i) > max)
			{
				max = object_intersections.at(i);
			}
		}
		// then starting from the maximum value find the minimum position
		double min_positive = max;
		if (max > 0)
		{
			//we only want positive intersections
			for (int index = 0; index < object_intersections.size(); index++)
			{
				if (object_intersections.at(index) > 0 && object_intersections.at(index) <= min_positive)
				{
					min_positive = object_intersections.at(index);
					index_of_minimum_value = index;
				}
			}
			return index_of_minimum_value;
		}
		else
		{
			//all the intersections are negative
			return -1;
		}
	}
}

Color getColorAt(Vect intersection_position, Vect intersection_ray_direction, vector<Object*> scene_objects, int index_of_winning_object, vector<Source*> light_sources, double accuracy, double ambientlight)
{
	Color winning_object_color = scene_objects.at(index_of_winning_object)->getColor();
	Vect winning_object_normal = scene_objects.at(index_of_winning_object)->getNormalAt(intersection_position);

	if (winning_object_color.getColorSpecial() == 2)
	{	
		// create checkered color/tile pattern

		int square = (int)floor(intersection_position.getVectX()) + (int)floor(intersection_position.getVectZ());

		if((square % 2) == 0)
		{
			//black tile
			winning_object_color.setColorRed(0);
			winning_object_color.setColorGreen(0);
			winning_object_color.setColorBlue(0);
		}
		else
		{
			//white tile
			winning_object_color.setColorRed(1);
			winning_object_color.setColorGreen(1);
			winning_object_color.setColorBlue(1);

		}
	}

	Color final_color = winning_object_color.colorScalar(ambientlight);

	if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1)
	{
		//reflection from objects with specular intensity
		double dot1 = winning_object_normal.dotProduct(intersection_ray_direction.negative());
		Vect scalar1 = winning_object_normal.vectMult(dot1);
		Vect add1 = scalar1.vectAdd(intersection_ray_direction);
		Vect scalar2 = add1.vectMult(2);
		Vect add2 = intersection_ray_direction.negative().vectAdd(scalar2);
		Vect reflection_direction = add2.normalize();
		
		Ray reflection_ray (intersection_position, reflection_direction);

		// determine what the ray intersects with first
		vector<double> reflection_intersections;

		for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++)
		{
			reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));
		}

		int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);

		if (index_of_winning_object_with_reflection != -1)
		{
			//reflection ray missed everything else
			if (reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy)
			{
				//determine position and direction at the point of intersection
				//the ray only affects the color if it reflects off something

				Vect reflection_intersection_position = intersection_position.vectAdd(reflection_direction.vectMult(reflection_intersections.at(index_of_winning_object_with_reflection)));
				Vect reflection_intersection_ray_direction = reflection_direction;

				Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersection_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);

				final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(winning_object_color.getColorSpecial()));
			}
		}
	}


	for (int light_index = 0; light_index < light_sources.size(); light_index++)
	{
		Vect light_direction = light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize();
		
		float cosine_angle = winning_object_normal.dotProduct(light_direction);

		if (cosine_angle > 0)
		{
			//test for shadows
			bool shadowed = false;

			Vect distance_to_light = light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize();
			float distance_to_light_magnitude = distance_to_light.magnitude();

			// ray in direction of light source
			Ray shadow_ray (intersection_position, light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize());
			
			vector<double> secondary_intersections;

			for (int object_index = 0; object_index < scene_objects.size() && shadowed == false; object_index++)
			{
				secondary_intersections.push_back(scene_objects.at(object_index)->findIntersection(shadow_ray));
			}

			for (int c = 0; c < secondary_intersections.size(); c++)
			{
				if(secondary_intersections.at(c) > accuracy)
				{
					if(secondary_intersections.at(c) <= distance_to_light_magnitude)
					{
						shadowed = true;
						break;
					}
				}
			}
			if (shadowed == false)
			{
				final_color = final_color.colorAdd(winning_object_color.colorMult(light_sources.at(light_index)->getLightColor()).colorScalar(cosine_angle));

				if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1)
				{
					//special [0, 1]
					double dot1 = winning_object_normal.dotProduct(intersection_ray_direction.negative());
					Vect scalar1 = winning_object_normal.vectMult(dot1);
					Vect add1 = scalar1.vectAdd(intersection_ray_direction);
					Vect scalar2 = add1.vectMult(2);
					Vect add2 = intersection_ray_direction.negative().vectAdd(scalar2);
					Vect reflection_direction = add2.normalize();

					double specular = reflection_direction.dotProduct(light_direction);
					if (specular > 0)
					{
						specular = pow(specular, 10);
						final_color = final_color.colorAdd(light_sources.at(light_index)->getLightColor().colorScalar(specular*winning_object_color.getColorSpecial()));
					}

				}


			}
		}

	}
	return final_color.clip();
}

//int thisone;

vector<Object*> scene_objects;
void makeCube (Vect corner1, Vect corner2, Color color)
{
	//corner 1
	double c1x = corner1.getVectX();
	double c1y = corner1.getVectY();
	double c1z = corner1.getVectZ();
	//corner 2
	double c2x = corner2.getVectX();
	double c2y = corner2.getVectY();
	double c2z = corner2.getVectZ();
	
	Vect A (c2x, c1y, c1z);
	Vect B (c2x, c1y, c2z);
	Vect C (c1x, c1y, c2z);

	Vect D (c2x, c2y, c1z);
	Vect E (c1x, c2y, c1z);
	Vect F (c1x, c2y, c2z);

	// left side
	scene_objects.push_back(new Triangle(D, A, corner1, color));
	scene_objects.push_back(new Triangle(corner1, E, D, color));
	// far side
	scene_objects.push_back(new Triangle(corner2, B, A, color));
	scene_objects.push_back(new Triangle(A, D, corner2, color));
	// right side
	scene_objects.push_back(new Triangle(F, C, B, color));
	scene_objects.push_back(new Triangle(B, corner2, F, color));
	//front side
	scene_objects.push_back(new Triangle(E, corner1, C, color));
	scene_objects.push_back(new Triangle(C, F, E, color));
	//top side
	scene_objects.push_back(new Triangle(D, E, F, color));
	scene_objects.push_back(new Triangle(F, corner2, D, color));
	//bottom side
	scene_objects.push_back(new Triangle(corner1, A, B, color));
	scene_objects.push_back(new Triangle(B, C, corner1, color));
}

int main(int argc, char *argv[])
{
	cout << "rendering ..." << endl;

	clock_t t1, t2;
	t1 = clock();

	int dpi = 72;
	int width = 800;
	int height = 600;
	int n = width*height;
	RGBType *pixels = new RGBType[n];

	int aadepth = 1; //anti-aliasing depth 1: 4 new rays at each pixel
	double aathreshold = 0.1;

	double aspectratio = (double)width / (double)height;
	double accuracy = 0.000001; //accuracy for ray intersection to be on outside of sphere


	// origin vector
	Vect O (0, 0, 0);

	Vect new_sphere_location(1.75, 0, 0);

	// three vectors for directions
	Vect X (1, 0, 0);
	Vect Y (0, 1, 0);
	Vect Z (0, 0, 1);

	//<camera>
    	//	<position x="0" y="-70" z="15"/>
    	//	<target x="2" y="0" z="3"/>
    	//	<up x="0" y="0" z="1"/>
    	//	<fov value="30"/>
    	//	<width value="800"/>
    	//	<height value="600"/>
  	//</camera>

	Vect campos (3, 1.5, -4);
	Vect look_at (0, 0, 0);

	Vect diff_btw (campos.getVectX() - look_at.getVectX(), campos.getVectY() - look_at.getVectY(), campos.getVectZ() - look_at.getVectZ());

	Vect camdir = diff_btw.negative().normalize();
	Vect camright = Y.crossProduct(camdir).normalize();
	Vect camdown = camright.crossProduct(camdir);

	Camera scene_cam (campos, camdir, camright, camdown);

	Color white_light (1.0, 1.0, 1.0, 0.0);
	Color pretty_green (0.5, 1.0, 0.5, 0.3);
	Color maroon (0.5, 0.25, 0.25, 0.3);
	Color orange (0.94, 0.75, 0.31, 0);
	Color tile_floor (1, 1, 1, 2);
	Color gray (0.5, 0.5, 0.5, 0);
	Color black (0.0, 0.0, 0.0, 0.0);

	//<!-- Lights -->
    	//<light type="ambient" name="ambientLight">
      	//	<intensity value="0.2"/>
    	//</light>
    	//<light type="direct" name="directLight">
      	//	<intensity value="0.6"/>
      	//	<direction x="-1" y="0.2" z="-1"/>
    	//</light>
    	//<light type="direct" name="directLight">
      	//	<intensity value="0.4"/>
      	//	<direction x="1" y="0.3" z="-1"/>
    	//</light>

	double ambientlight = 0.2;

	Vect light_position (-7, 10, -10);
	Light scene_light (light_position, white_light);
	vector<Source*> light_sources;
	light_sources.push_back(dynamic_cast<Source*>(&scene_light));

	// Y is the up-down direction, Z is the depth, X is the left-right

	//<!-- Objects -->
    	//<object type="plane" name="groundPlane" material="ground">
      	//	<scale value="1000"/>
      	//	<rotate angle="30" z="1"/>
    	//</object>
    	//<object type="obj" name="teapot.obj" material="bricks">
      	//	<scale value="0.8"/>
      	//	<rotate angle="-50" z="1"/>
      	//	<translate x="2" y="-5" z="0"/>
    	//</object>
    	//<object type="sphere" name="sphere1" material="checkerMtl">
      	//	<scale value="6"/>
      	//	<translate x="15" y="2" z="6"/>
    	//</object>
    	//<object type="sphere" name="sphere2" material="refractive">
      	//	<scale value="5"/>
      	//	<translate x="-8" y="-16" z="5"/>
    	//</object>


	//scene objects
	Vect scene_sphere1_center (15, 2, 6);
	Sphere scene_sphere1 (O, 1.0, pretty_green);
	Vect scene_sphere2_center (-8, -16, 5);
	Sphere scene_sphere2 (new_sphere_location, 0.5, maroon);
	Plane scene_plane (Y, -1, tile_floor);
	Triangle scene_triangle (Vect (3, 0, 0), Vect (0, 3, 0), Vect (0, 0, 3), orange);

	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere1));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere2));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_plane));
	//scene_objects.push_back(dynamic_cast<Object*>(&scene_triangle));
	
	//makeCube(Vect(1, 1, 1), Vect(-1, -1, -1), orange);

	int thisone, aa_index;
	double xamnt, yamnt;
	double tempRed, tempGreen, tempBlue;

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			thisone = y*width + x;


			//start with a blank pixel
			double tempRed[aadepth*aadepth];
			double tempGreen[aadepth*aadepth];
			double tempBlue[aadepth*aadepth];

			

			for (int aax =0; aax < aadepth; aax++)
			{
				for(int aay = 0; aay < aadepth; aay++)
				{
					aa_index = aay*aadepth + aax;
					
					srand(time(0));

					//create the ray from the camera to this pixel
					if (aadepth == 1)
					{
						// start with no anti-aliasing
						if (width > height)
						{
							//the image is wider than it is tall
							xamnt = ( (x+0.5)/ width) * aspectratio - (( (width - height) / (double)height)/2);
							yamnt = ( (height - y) + 0.5)/ height;
						}
						else if (height > width)
						{
							//the image is taller than it is wide
							xamnt = ( x + 0.5) /width;
							yamnt = (((height - y) + 0.5)/height)/aspectratio - (((height - width)/(double)width/2));
						}
						else
						{
							//the image is square
							xamnt = (x + 0.5) / width;
							yamnt = (y + 0.5) / height;
						}
					}
					else
					{
						//anti-aliasing
						if (width > height)
						{
							//the image is wider than it is tall
							xamnt = ( (x + (double)aax/((double)aadepth - 1))/ width) * aspectratio - (( (width - height) / (double)height)/2);
							yamnt = ( (height - y) + (double)aax/((double)aadepth - 1))/ height;
						}
						else if (height > width)
						{
							//the image is taller than it is wide
							xamnt = ( x + (double)aax/((double)aadepth - 1)) /width;
							yamnt = (((height - y) + (double)aax/((double)aadepth - 1))/height)/aspectratio - (((height - width)/(double)width/2));
						}
						else
						{
							//the image is square
							xamnt = (x + (double)aax/((double)aadepth - 1)) / width;
							yamnt = (y + (double)aax/((double)aadepth - 1)) / height;
						}
					}
		
					Vect cam_ray_origin = scene_cam.getCameraPosition();
					Vect cam_ray_direction = camdir.vectAdd(camright.vectMult(xamnt - 0.5).vectAdd(camdown.vectMult(yamnt - 0.5))).normalize();
					Ray cam_ray (cam_ray_origin, cam_ray_direction);
		
					vector<double> intersections;
		
					for (int index = 0; index < scene_objects.size(); index++)
					{
						intersections.push_back(scene_objects.at(index)->findIntersection(cam_ray));
					}
		
					int index_of_winning_object = winningObjectIndex(intersections);
		
					if ( index_of_winning_object == -1 )
					{
						//set the background black
						tempRed[aa_index] = 0;
						tempGreen[aa_index] = 0;
						tempBlue[aa_index] = 0;
					}
					else
					{
						// index corresponds to an object in our scene
						if (intersections.at(index_of_winning_object) > accuracy)
						{   
							//determine the position and direction vectors at point of intersection
		
								Vect intersection_position = cam_ray_origin.vectAdd(cam_ray_direction.vectMult(intersections.at(index_of_winning_object)));
							Vect intersection_ray_direction = cam_ray_direction;
		
							Color intersection_color = getColorAt(intersection_position, intersection_ray_direction, scene_objects, index_of_winning_object, light_sources, accuracy, ambientlight);
		
							tempRed[aa_index] = intersection_color.getColorRed();
							tempGreen[aa_index] = intersection_color.getColorGreen();
							tempBlue[aa_index] = intersection_color.getColorBlue();
						}
					}
				}
			}

			//average the pixel color
			double totalRed = 0;
			double totalGreen = 0;
			double totalBlue = 0;

			for (int iRed = 0; iRed < aadepth*aadepth; iRed++)
			{
				totalRed = totalRed + tempRed[iRed];
			}

			for (int iGreen = 0; iGreen < aadepth*aadepth; iGreen++)
			{
				totalGreen = totalGreen + tempGreen[iGreen];
			}

			for (int iBlue = 0; iBlue < aadepth*aadepth; iBlue++)
			{
				totalBlue = totalBlue + tempBlue[iBlue];
			}

			double avgRed = totalRed/(aadepth*aadepth);
			double avgGreen = totalGreen/(aadepth*aadepth);
			double avgBlue = totalBlue/(aadepth*aadepth);

			pixels[thisone].r = avgRed;
			pixels[thisone].g = avgGreen;
			pixels[thisone].b = avgBlue;
		}
	}

	savebmp("scene_anti-aliased12.bmp", width, height, dpi, pixels);

	delete[] pixels;
	//delete tempRed, tempGreen, tempBlue;

	t2 = clock();
	float diff = ((float)t2 - (float)t1)/1000;
	cout << diff << " seconds" << endl;

	return 0;
}


