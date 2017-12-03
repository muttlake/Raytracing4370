#ifndef TEXTUREH
#define TEXTUREH 

#include <glm_dir/glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm_dir/glm/gtx/rotate_vector.hpp>


class texture
{
	public:
		virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class constant_texture: public texture
{
	public:
		constant_texture() {}
		constant_texture(vec3 c): color(c) {}
		virtual vec3 value(float u, float v, const vec3& p) const
		{
			return color;
		}
		vec3 color;
};

class checker_texture: public texture
{
	public:
		checker_texture() { }
		checker_texture(texture *t0, texture *t1, float s): even(t0), odd(t1), sine_multiplier(s) { }

		virtual vec3 value(float u, float v, const vec3& p) const
		{
			float sines = sin(sine_multiplier*p.x() ) * sin(sine_multiplier*p.y()) * sin(sine_multiplier*p.z());
			if (sines < 0)
				return odd->value(u, v, p);
			else
				return even->value(u, v, p);
		}

		float sine_multiplier;

		texture *odd;
		texture *even;
};


class plane_checker_texture: public texture
{
	public:
		plane_checker_texture() { }
		plane_checker_texture(texture *t0, texture *t1, float _r): even(t0), odd(t1), r(_r) { }

		virtual vec3 value(float u, float v, const vec3& p) const
		{
			//glm::vec3 xyVector = glm::vec3(p.x(), p.y(), 0.0);
			//glm::mat4 R = glm::gtx::rotate(glm::radians(r), glm::vec3(0.0, 0.0, 1.0));
			//glm::vec3 rotated_xyVector = R * xyVector;

			glm::vec2 xyVector = glm::vec2(p.x(), p.y());
			glm::vec2 rotated_xyVector = glm::rotate(xyVector, glm::radians(r));
			
			//int square = (int)floor(p.x()*0.1) + (int)floor(p.y()*0.1);
			int square = (int)floor(rotated_xyVector[0]*0.1) + (int)floor(rotated_xyVector[1]*0.1);

			if ((square % 2) == 0)
				return odd->value(u, v, p);
			else
				return even->value(u, v, p);
		}

		float r;
		texture *odd;
		texture *even;
};


class image_texture : public texture
{
	public:
		image_texture() { }
		image_texture( unsigned char *pixels, int A, int B) : data(pixels), nx(A), ny(B) { }
		virtual vec3 value(float u, float v, const vec3& p) const;
		unsigned char *data;
		int nx, ny;
};


vec3 image_texture::value(float u, float v, const vec3& p) const
{
	int i = (  u)*nx;
	int j = (1-v)*ny - 0.001;
	if (i < 0) i = 0;
	if (j < 0) j = 0;
	if (i > nx-1) i = nx - 1;
	if (j > ny-1) j = ny - 1;
	//int i = 0.5*nx;
	//int j = 0.5*ny;
	float r = int(data[3*i + 3*nx*j]  ) / 255.0;
	float g = int(data[3*i + 3*nx*j+1]) / 255.0;
	float b = int(data[3*i + 3*nx*j+2]) / 255.0;
	//std::cout << "Image Texture: u: " << u << " , v: " << v;
	//std::cout << " r: " << r << " , g: " << g << " , b: " << b << "\n";
	return vec3(r, g, b);
}




#endif




