#define _USE_MATH_DEFINES

#include <iostream>
#include "header/ray.h"
#include "header/hitablelist.h"
#include "header/sphere.h"
#include <float.h>
#include <cmath>
#include "header/camera.h"
#include "header/material.h"
#include "header/moving_sphere.h"
#include "header/aa_rect.h"
#include "header/box.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "header/stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "header/stb_image.h"

//#define drand48() (rand()%(100)/(float)(100))
//float drand48() { return (rand()%(100)/(float)(100)); }

vec3 color(const ray& r, hitable* world,int depth)
{
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec))   //这里用"0.001"而不是"0.00"是为了避免shadow acne 问题
	{
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		{
			return emitted + attenuation * color(scattered, world, depth + 1);
		}
		else
		{
			return emitted;
		}
	}
	else
	{
		return vec3(0, 0, 0);
	}
}

hitable* cornell_box()
{
	hitable** list = new hitable * [8];
	int i = 0;
	material* red = new lambertial(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material* white = new lambertial(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material* green = new lambertial(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material* light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, red));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, green);
	list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));

	return new hitable_list(list, i);
}

hitable* random_scene()
{
	int n = 500;
	hitable** list = new hitable * [n + 1];
	texture* checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertial(checker));
	int i = 1;
	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			float choose_mat = drand48();
			vec3 center(a + 0.9 * drand48(), 0.2, b + 0.9 * drand48());
			if ((center - vec3(4, 0.2, 0)).length() > 0.9)
			{
				if (choose_mat < 0.8)
				{
					//diffuse
					list[i++] = new moving_sphere(center, center + vec3(0, 0.5 * drand48(), 0), 0.0, 1.0, 0.2, new lambertial(new constant_texture(vec3(drand48() * drand48(), drand48() * drand48(), drand48() * drand48()))));
				}
				else if (choose_mat < 0.95)
				{
					//metal
					list[i++] = new sphere(center, 0.2,
						new metal(vec3(0.5 * (1 + drand48()), 0.5 * (1 + drand48()), 0.5 * (1 + drand48())), 0.5 * drand48()));
				}
				else
				{
					//glass
					list[i++] = new sphere(center, 0.2, new dielectric(1.5));
				}
			}
		}
	}
	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertial(new constant_texture(vec3(0.4, 0.2, 0.1))));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

	return new hitable_list(list, i);
}


struct RGB
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

int main()
{
	const int nx = 500;
	const int ny = 300;
	const int ns = 500;
	std::cout << "P3\n" << nx << "" << ny << "\n255\n";
	RGB data[ny][nx];
	/*hitable* list[5];
	list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertial(new constant_texture(vec3(0.1, 0.2, 0.5))));
	list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertial(new constant_texture(vec3(0.8, 0.8, 0.0))));
	list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2),0.2));
	list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
	list[4] = new sphere(vec3(-1, 0, -1), -0.45, new dielectric(1.5));
	hitable* world = new hitable_list(list, 5);*/
	hitable* world = cornell_box();
	
	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	//float dist_to_focus = (lookfrom - lookat).length();
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 40.0;
	camera cam(lookfrom, lookat, vec3(0, 1, 0), vfov, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);

	int flag = 0;

	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i < nx; i++)
		{
			vec3 col(0, 0, 0);
			for (int s = 0; s < ns; s++)
			{
				float u = float(i + rand() / (RAND_MAX + 1.0)) / float(nx);
				float v = float(j + rand() / (RAND_MAX + 1.0)) / float(ny);
				ray r = cam.get_ray(u, v);
				//vec3 p = r.point_at_parameter(2.0);
				col += color(r, world,0);
			}
			col /= float(ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2])); //gamma 2
			int ir = int(255.99 * col[0]);
			int ig = int(255.99 * col[1]);
			int ib = int(255.99 * col[2]);
			data[flag][i].r = ir;
			data[flag][i].g = ig;
			data[flag][i].b = ib;
		}
		flag++;
	}
	stbi_write_jpg("Cornell Box(ver 3).jpg", nx, ny, 3,data, 100);
}
