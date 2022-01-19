#include <iostream>
#include <vector>
#include <cmath>
#include <complex>
#include <fstream>

using Comp = std::complex<float>;

extern "C" {
    #define STB_IMAGE_IMPLEMENTATION
    #include "stb_image.h"
}

bool load_image(std::vector<unsigned char>& image, const std::string& filename, int& x, int&y)
{
    int n;
    unsigned char* data = stbi_load(filename.c_str(), &x, &y, &n, 4);
    if (data != nullptr)
    {
        image = std::vector<unsigned char>(data, data + x * y * 4);
    }
    stbi_image_free(data);
    return (data != nullptr);
}

class Polynomial{
public:
	std::vector<Comp> roots;
	int n = 0;

	Polynomial() {}

	void addRoot(Comp root) {
		roots.push_back(root);
		n++;
	}

	Comp derivative(Comp x) {
		Comp result;

		for(int i = 0; i < n; i++) {
			Comp a = x + roots[i==0?1:0];

			for(int j = i==0?2:1; j < n; j++)
				if(j != i)
					a *= x + roots[j];

			result += a;
		}

		return result;
	}

	Comp get(Comp x) {
		Comp result = 1;

		for(int i = 0; i < n; i++) {
			result *= x + roots[i];
		}

		return result;
	}

	Comp newtonsMethod(Comp x_, int iters=100) {
		Comp x = x_;
		for (int i = 0; i < iters; i++) {
			Comp d = derivative(x);

			if (std::real(d) != 0) {
				x = x - get(x)/d;
			}
		}

		return x;
	}

	int findNearestRoot(Comp x_, int iters=100) {
		Comp x = newtonsMethod(x_, iters);

		int r = 0;
		float d = 1000000.0f;

		int i = 0;
		for(Comp root : roots) {
			float xx = (float)std::real(root)-(float)std::real(x);
			float yy = (float)std::imag(root)-(float)std::imag(x);

			float dn = xx*xx+yy*yy;

			if(dn < d) {
				d = dn;
				r = i;
			}
			i++;
		}

		return r;
	}
};

int clip(int a, int b, int c){
	return (a > c) ? c : (a < b) ? b : a;
}

float clipf(float a, float b, float c) {
	return (a > c) ? c : (a < b) ? b : a;
}

int clip256(int a) {
	return (a > 255) ? 255 : (a < 0) ? 0 : a;
}

struct Img{
	std::vector<unsigned char> pixels;
	int width;
	int height;
};

Img getImage() {
    std::string filename = "in.png";
    
    int width, height;
    std::vector<unsigned char> image;

    bool success = load_image(image, filename, width, height);
    if (!success) std::cout << "Error loading image\n";
	else std::cout << "image loaded successfuly!\n";

	Img img = {
		image,
		width, 
		height
	};

	return img;
}

void complexSample(Img img, Comp p, int* s, float scale = 1.0f) {
	int	x = clip((std::real(p)/scale/2.0f+0.5f)*img.width, 0, img.width),
		y = clip((std::imag(p)/scale/2.0f+0.5f)*img.height, 0, img.height);

	s[0] = img.pixels[4*(x+y*img.width)+0];
	s[1] = img.pixels[4*(x+y*img.width)+1];
	s[2] = img.pixels[4*(x+y*img.width)+2];
	s[3] = img.pixels[4*(x+y*img.width)+3];
}

int main() {
	srand(69);
	const int	Width = 256,
				Height = 256;

	const int  total_colors = 100;
	int colors[total_colors][3];

	for(int i = 0; i < total_colors; i++)
		for(int j = 0; j < 3; j++) {
			int c = rand()%16;
			colors[i][j] = 255-c*c;
		}

	Img image = getImage();

	std::ofstream out;
	out = std::ofstream("out.ppm");
	out << "P3\n"
		<< Width << ' ' << Height << ' ' << "255\n";
		
	Polynomial p;
	
	float total = 20.0f;

	for (int i = 0; i < total; i++){
		float t = (float)i/total;
		float a = (1.0f-t) * 2.0f * 3.1415926535f;

		std::cout 	<< "t: " << t << "\n"
					<< "a: " << a << "\n";
		std::cout 	<< "x: " << cos(a) << "\n"
					<< "y: " << sin(a) << "\n\n";

		p.addRoot(Comp(
			cos(a) * 0.5f * (i%2+1) * (i%4+1),
			sin(a) * 0.5f * (i%2+1) * (i%4+1)
		));
	}

	for (int y = 0; y < Height; y++){
		for (int x = 0; x < Width; x++)	{
			Comp cp = p.newtonsMethod(
				Comp(
					x/float(Width)*4.0f-2.0f, 
					y/float(Height)*4.0f-2.0f
				), 21);

			// int c[4];
			// complexSample(image, cp, c, 8.0f);

			// out << c[0] << " " << c[1] << " " << c[2] << "\n";

			int r = clipf(std::imag(cp) /  4.0f + 0.0f, 0.0f, 1.0f)*255;
			int g = clipf(std::imag(cp) / -4.0f + 0.0f, 0.0f, 1.0f)*255;
			int b = clipf(std::imag(cp) / -4.0f + 0.0f, 0.0f, 1.0f)*255;//clipf(std::real(cp) /  4.0f + 0.0f, 0.0f, 1.0f)*255;

			out << clip256(r) << ' '
				<< clip256(g) << ' '
				<< clip256(b) << "\n";
		}
		std::cout << (float)y/(float)Height*100.0f << "%\n";
	}
}
