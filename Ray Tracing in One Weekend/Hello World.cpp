#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct RGB
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

int main()
{
	const int nx = 200;
	const int ny = 100;
	RGB data[ny][nx]; //md,ppm转成jpg得反过来，就是长和宽的存储信息是反过来的
	int flag = 0;
	
	std::cout << "P3\n" << nx << " " << ny << "\n255\n";
	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i < nx; i++)
		{
			float r = float(i) / float(nx);
			float g = float(j) / float(ny);
			float b = 0.2;
			int ir = int(255.99 * r);
			int ig = int(255.99 * g);
			int ib = int(255.99 * b);
			data[flag][i].r = ir;
			data[flag][i].g = ig;
			data[flag][i].b = ib;
			//std::cout << ir << " " << ig << " " << ib << "\n";			
		}
		flag++;
	}
	stbi_write_jpg("test.jpg", nx, ny, 3, data, 100);
}