#pragma once
#include <chrono>
#include "bitmap_image.hpp"
#include "vector3f.hpp"
#include "kdtree.hpp"
#include "primitive.hpp"
#include "ray_trace.h"

#define RANDOM ((rand() & 63) / 63.0)

using namespace std;
using namespace chrono;

void light_material_test() {
	material material_gold(gold);
	material material_glass(glass);
	material material_silver(silver);
	material material_white_plastic(white_plastic);
	triangle a(vector3f(0, 0, 5), vector3f(5, 0, 0), vector3f(0, 5, 0), material_gold);
	//triangle b(vector3f(9, 10, 10), vector3f(10, 10, 9), vector3f(10, 9, 10), material_gold);
	//triangle c(vector3f(10, 0, 0), vector3f(10, 0, 2), vector3f(10, 2, 0), material_gold);
	triangle ta(vector3f(0, 0, 0), vector3f(10, 0, 0), vector3f(0, 10, 0), material_gold);
	triangle tb(vector3f(10, 0, 0), vector3f(10, 10, 0), vector3f(0, 10, 0), material_gold);
	triangle la(vector3f(3, 21, 18), vector3f(11, 21, 18), vector3f(3, 21, 26), material_white_plastic);
	triangle lb(vector3f(11, 21, 18), vector3f(11, 21, 26), vector3f(3, 21, 26), material_white_plastic);
	ball d(vector3f(1, 9, 1), 1.0, material_silver);
	ball e(vector3f(3, 7, 1), 1.0, material_glass);

	vector<primitive*> asdf;
	asdf.push_back(&a);
	//asdf.push_back(&b);
	//asdf.push_back(&c);
	asdf.push_back(&d);
	asdf.push_back(&e);
	asdf.push_back(&ta);
	asdf.push_back(&tb);
	asdf.push_back(&la);
	asdf.push_back(&lb);

	aabb box(vector3f(-eps2, -eps2, -eps2), vector3f(30 + eps2, 30 + eps2, 30 + eps2));
	
	cout << "building kdtree.. ";
	auto begin = high_resolution_clock::now();
	kdtree kd(box, asdf, 0);
	auto end = high_resolution_clock::now();
	cout << "done in " << duration_cast<milliseconds>(end - begin).count() << "ms" << endl;



	vector3f cam(11, 11, 11);
	vector3f center(0, 0, 0);
	vector3f dir = (center - cam).normal();
	double t = vector3f(0, 0, 1) * dir / -(dir.norm() * dir.norm());
	vector3f up = (dir * t + vector3f(0, 0, 1)).normal();
	const double pi = 3.141592653589793;
	double fovy = 45.0 * 2 * pi / 360.0;
	int vres = 1080, hres = 1920;
	double delta = 2.0 * tan(fovy / 2.0) / vres;
	vector3f u = up ^ dir;
	vector3f v = up;
	double v_base = -tan(fovy / 2.0);
	double u_base = v_base * hres / vres;

	bitmap_image image(hres, vres);


	cout << "casting rays.. " << endl;
	begin = high_resolution_clock::now();
	for (int i = 0; i < hres; i++)
	{
		cout << "\tprogress: " << (double)i / (double)hres * 100 << "%" << endl;
		for (int j = 0; j < vres; j++)
		{
			vector3f rgb(0, 0, 0);
			for (int k = 0; k < 9; k++)
			{
				ray r(cam, dir + (u_base + (i + RANDOM) * delta) * u + (v_base + (j + RANDOM) * delta) * v);
				rgb += get_ray_trace(r, kd);
			}
			rgb = rgb / 9;
			image.set_pixel(hres - i - 1, vres - j - 1, rgb[0] * 255, rgb[1] * 255, rgb[2] * 255);
		}
	}
	end = high_resolution_clock::now();
	cout << "done in " << duration_cast<milliseconds>(end - begin).count() << "ms" << endl;
	image.save_image("output\\test3.bmp");

	char asdfef;
	cin >> asdfef;
}

void kdtree_test() {
	triangle a(vector3f(0, 0, 5), vector3f(5, 0, 0), vector3f(0, 5, 0));
	triangle b(vector3f(9, 10, 10), vector3f(10, 10, 9), vector3f(10, 9, 10));
	triangle c(vector3f(10, 0, 0), vector3f(10, 0, 2), vector3f(10, 2, 0));
	ball d(vector3f(1, 9, 1), 1.0);

	vector<primitive*> asdf;
	asdf.push_back(&a);
	asdf.push_back(&b);
	asdf.push_back(&c);
	asdf.push_back(&d);

	aabb box(vector3f(-eps2, -eps2, -eps2), vector3f(10 + eps2, 10 + eps2, 10 + eps2));

	kdtree kd(box, asdf, 0);

	ray r2(vector3f(5, 5, 20), vector3f(-5, -5, -20));
	primitive *result = get_intersecting_primitive(kd, r2);

	ray r3(vector3f(20, 20, 20), vector3f(-1, -1, -1));
	primitive *result2 = get_intersecting_primitive(kd, r3);

	ray r4(vector3f(20, 20, 20), vector3f(-10, -19.5, -19.5));
	primitive *result3 = get_intersecting_primitive(kd, r4);

	ray r5(vector3f(20, 20, 20), vector3f(-19.5, -10, -19.5));
	primitive *result4 = get_intersecting_primitive(kd, r5);

	vector3f cam(20, 20, 20);
	vector3f dir = vector3f(-1, -1, -1).normal();
	vector3f up = vector3f(-1, -1, 1).normal();
	const double pi = 3.141592653589793;
	double fovy = 45.0 * 2 * pi / 360.0;
	int vres = 1080, hres = 1920;
	double delta = 2.0 * tan(fovy / 2.0) / vres;
	vector3f u = up ^ dir;
	vector3f v = up;
	double v_base = -tan(fovy / 2.0);
	double u_base = v_base * hres / vres;

	bitmap_image image(hres, vres);
	for (int i = 0; i < hres; i++)
	{
		for (int j = 0; j < vres; j++)
		{
			ray r(cam, dir + (u_base + i * delta) * u + (v_base + j * delta) * v);
			primitive *result = get_intersecting_primitive(kd, r);
			if (result != NULL) {
				int red=255, green=255, blue=255;
				image.set_pixel(hres - i - 1, vres - j - 1, 255, 255, 255);
			}
			else {
				image.set_pixel(hres - i - 1, vres - j - 1, 0, 0, 0);
			}
		}
	}
	image.save_image("output\\test2.bmp");
}

void intersection_test()
{
	triangle poly(vector3f(0, 0, 0), vector3f(10, 0, 0), vector3f(0, 10, 0));
	ball b(vector3f(0, 0, 0), 5.0);
	ray r(vector3f(0, 0, 10), vector3f(1, 1, -5));
	double dist, asdf = 0;
	for (int t = 0; t < 10; t++) {
		auto begin = high_resolution_clock::now();
		for (int i = 0; i < 1180000000; i++) {
			dist = b.get_intersection_distance(r);
			asdf += i;
		}
		auto end = high_resolution_clock::now();
		cout << r.origin + dist * r.direction << endl;
		cout << asdf << endl;
		cout << duration_cast<milliseconds>(end - begin).count() << endl;
	}
	int a;
	cin >> a;
}

void image_save_test()
{
	bitmap_image image(255, 255);
	for (int i = 0; i < 255; i++)
	{
		for (int j = 0; j < 255; j++) 
		{
			image.set_pixel(i, j, i, j, (i + j) / 2);
		}
	}

	image.save_image("output\\test2.bmp");

}

int main()
{
	light_material_test();
}
