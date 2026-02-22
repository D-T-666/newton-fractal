#include <iostream>
#include <vector>
#include <cmath>
#include <complex>
#include <fstream>
#include <thread>

using Comp = std::complex<float>;

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
		Comp result = 0;

		Comp d[n + 1];
		d[n] = 1;
		for (int i = n - 1; i >= 1; i--) {
			d[i] = d[i + 1] * (x - roots[i]);
		}

		Comp t = 1;
		for(int i = 0; i < n; i++) {
			result += t * d[i + 1];
			t *= x - roots[i];
		}

		return result * 1000.0f;
	}

	Comp eval(Comp x) {
		Comp result = 1;

		for(int i = 0; i < n; i++) {
			result *= x - roots[i];
		}

		return result * 1000.0f;
	}

	Comp newtonsMethod(Comp x_0, int iters) {
		Comp x = x_0;
		for (int i = 0; i < iters; i++) {
			Comp der = derivative(x);

			if (std::real(der) != 0) {
				x = x - eval(x) / der;
			}
		}

		return x;
	}

	int findNearestRoot(Comp x_0, int iters) {
		Comp x = newtonsMethod(x_0, iters);

		int best = 0;
		for(int i = 1; i < roots.size(); i++) {
			if (std::norm(x - roots[i]) < std::norm(x - roots[best])) {
				best = i;
			}
		}

		return best;
	}
};

int clip_u8(int a) {
	return (a > 255) ? 255 : (a < 0) ? 0 : a;
}

const int	Height = 1600, Width = Height * 8 / 5;
const int total = 40;
const float radius = 1.0f;

void render_rows(Polynomial p, int start, int end, std::vector<int> &res) {
	for (int y = start; y < end; y++) {
		for (int x = 0; x < Width; x++)	{
			res[y * Width + x] = p.findNearestRoot(
				Comp(
					x / (float)Width  * 1.6f * radius - 0.8f * radius,
					y / (float)Height * 1.0f * radius - 0.5f * radius
				), 60);
		}
		std::cout << (float)(y - start)/(float)(end - start)*100.0f << "%\n";
	}
}

int main() {
	srand(420);

	int colors[total][3];

	for(int i = 0; i < total; i++) {
		for(int j = 0; j < 3; j++) {
			int c = rand() % 16;
			colors[i][j] = 255 - c * c;
		}
	}

	colors[0][0] = 0;
	colors[0][1] = 0;
	colors[0][2] = 0;

	std::ofstream out;
	out = std::ofstream("out.ppm");
	out << "P3\n"
		<< Width << ' ' << Height << ' ' << "255\n";

	Polynomial p;

	for (int i = 0; i < total; i++) {
		float t = (float)i / (float)total;
		float a = (float)i * 1.6182;
		float r = (i % 3 + 1) * (i % 5 + 1) * (i % 7 + 1) * (i % 11 + 1) * (i % 13 + 1) * (i % 17 + 1) / 10000.0;

		p.addRoot(Comp(
			cos(a) * r,
			sin(a) * r
		));
	}

	std::vector<int> result;
	result.reserve(Width * Height);

	int dr = Height / 8;
	std::thread t1(render_rows, p, dr * 0, dr * 0 + dr, std::ref(result));
	std::thread t2(render_rows, p, dr * 1, dr * 1 + dr, std::ref(result));
	std::thread t3(render_rows, p, dr * 2, dr * 2 + dr, std::ref(result));
	std::thread t4(render_rows, p, dr * 3, dr * 3 + dr, std::ref(result));
	std::thread t5(render_rows, p, dr * 4, dr * 4 + dr, std::ref(result));
	std::thread t6(render_rows, p, dr * 5, dr * 5 + dr, std::ref(result));
	std::thread t7(render_rows, p, dr * 6, dr * 6 + dr, std::ref(result));
	std::thread t8(render_rows, p, dr * 7, dr * 7 + dr, std::ref(result));

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();
	t7.join();
	t8.join();

	for (int y = 0; y < Height; y++) {
		for (int x = 0; x < Width; x++)	{
			int ind = result[y * Width + x];

			int *c;
			c = colors[ind];

			out << c[0] << " " << c[1] << " " << c[2] << "\n";
		}
	}
}
