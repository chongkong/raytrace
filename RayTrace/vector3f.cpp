#include "vector3f.hpp"

vector3f::vector3f() {
	vec[0] = vec[1] = vec[2] = 0;
}
vector3f::vector3f(double x, double y, double z) {
	vec[0] = x, vec[1] = y, vec[2] = z;
}
vector3f::vector3f(const vector3f &v) {
	vec[0] = v.vec[0], vec[1] = v.vec[1], vec[2] = v.vec[2];
}
vector3f::vector3f(const double *v) {
	vec[0] = v[0], vec[1] = v[1], vec[2] = v[2];
}

vector3f& vector3f::operator+=(const vector3f &w) {
	vec[0] += w.vec[0], vec[1] += w.vec[1], vec[2] += w.vec[2];
	return *this;
}
vector3f& vector3f::operator+=(double d) {
	vec[0] += d, vec[1] += d, vec[2] += d;
	return *this;
}
vector3f operator+(const vector3f &v, const vector3f &w) {
	return vector3f(v.vec[0] + w.vec[0], v.vec[1] + w.vec[1], v.vec[2] + w.vec[2]);
}
vector3f operator+(const vector3f &v, const double d) {
	return vector3f(v.vec[0] + d, v.vec[1] + d, v.vec[2] + d);
}
vector3f operator-(const vector3f &v, const vector3f &w) {
	return vector3f(v.vec[0] - w.vec[0], v.vec[1] - w.vec[1], v.vec[2] - w.vec[2]);
}
double operator*(const vector3f &v, const vector3f &w) {
	return (v.vec[0] * w.vec[0] + v.vec[1] * w.vec[1] + v.vec[2] * w.vec[2]);
}
vector3f operator*(const vector3f& v, const double c) {
	return vector3f(v.vec[0] * c, v.vec[1] * c, v.vec[2] * c);
}
vector3f operator*(const double c, const vector3f& v) {
	return v * c;
}
vector3f operator/(const vector3f &v, const double c) {
	return v * (1.0 / c);
}
vector3f operator^(const vector3f &v, const vector3f &w) {
	return vector3f(
		v.vec[1] * w.vec[2] - v.vec[2] * w.vec[1],
		v.vec[2] * w.vec[0] - v.vec[0] * w.vec[2],
		v.vec[0] * w.vec[1] - v.vec[1] * w.vec[0]
		);
}
double& vector3f::operator[](unsigned index) {
	return vec[index];
}

double vector3f::norm() const {
	return sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
}
vector3f vector3f::normal() const {
	return (*this) / this->norm();
}

double vector3f::min3() const {
	return min(vec[0], min(vec[1], vec[2]));
}
double vector3f::max3() const {
	return max(vec[0], max(vec[1], vec[2]));
}

std::ostream& operator<< (std::ostream &os, const vector3f &v) {
	os << "(" << v.vec[0] << ", " << v.vec[1] << ", " << v.vec[2] << ")";
	return os;
}
std::istream& operator>> (std::istream &is, vector3f &v) {
	is >> v.vec[0] >> v.vec[1] >> v.vec[2];
	return is;
}

vector3f map_mul(vector3f &v, vector3f &w) {
	return vector3f(v[0] * w[0], v[1] * w[1], v[2] * w[2]);
}