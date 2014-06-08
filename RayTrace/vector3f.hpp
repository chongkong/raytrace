#pragma once
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

const double eps = 1e-9;
const double eps2 = 1e-6;


class vector3f {
public:
	double vec[3];

	vector3f();
	vector3f(double x, double y, double z);
	vector3f(const vector3f &v);
	vector3f(const double *v);

	double norm() const;
	vector3f normal() const;

	friend std::ostream& operator<< (std::ostream &os, const vector3f &v);
	friend std::istream& operator>> (std::istream &is, vector3f &v);

	vector3f& operator+=(const vector3f &w);
	vector3f& operator+=(double d);
	double& operator[] (const unsigned index);
	double min3() const;
	double max3() const;
};



vector3f operator+(const vector3f &v, const vector3f &w);
vector3f operator+(const vector3f &v, const double d);

vector3f operator-(const vector3f &v, const vector3f &w);
double operator*(const vector3f &v, const vector3f &w);
vector3f operator*(const vector3f& v, const double c);
vector3f operator*(const double c, const vector3f& v);
vector3f operator/(const vector3f &v, const double c);
vector3f operator^(const vector3f &v, const vector3f &w);

vector3f map_mul(vector3f &v, vector3f &w);