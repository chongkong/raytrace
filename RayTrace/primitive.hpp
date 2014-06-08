#pragma once
#include <vector>
#include <algorithm>
#include "vector3f.hpp"
#include "material.h"

using namespace std;

/* ray */
class ray 
{
public:
	vector3f origin;
	vector3f direction;

	ray(vector3f orig, vector3f dir);
};

/* axis aligned bounding box */
class aabb 
{
public:
	vector3f min, max;

	aabb();
	aabb(double minx, double miny, double minz, double maxx, double maxy, double maxz);
	aabb(vector3f minimum, vector3f maximum);

	aabb& operator=(const aabb &another);

	bool is_inside(vector3f point) const;
	void split_aabb(int dimension, double plane, aabb &result_left, aabb &result_right) const;
	double get_surface_area() const;
};

aabb aabb_intersection(const aabb &first, const aabb &second);

class primitive 
{
private:
	material *mat;
public:
	virtual double get_intersection_distance(ray &r) = 0;
	virtual aabb& get_aabb() = 0;
	virtual vector3f get_normal(vector3f point) = 0;
	virtual void set_material(material &material) {
		mat = &material;
	}
	virtual material& get_material() {
		return *mat;
	}
};

class triangle : public primitive 
{
private:
	aabb bbox;
	vector<vector3f> vertex_normal;
	vector3f barycentric_normal;
	struct tri_accel {
		int u, v;
		double anu, anv;
		double bnu, bnv;
		double distance;
		vector3f normal;
	} ta;

public:
	vector<vector3f> vertex;

	triangle();
	triangle(vector3f va, vector3f vb, vector3f vc);
	triangle(vector3f va, vector3f vb, vector3f vc, material &material) : triangle(va, vb, vc) {
		set_material(material);
	}

	double get_intersection_distance(ray &r);
	aabb& get_aabb();
	vector3f get_normal(vector3f point);
};

class ball : public primitive{
private:
	aabb bbox;
	vector3f center;
	double radius;

public:
	ball(vector3f center, double radius);
	ball(vector3f center, double radius, material &material) : ball(center, radius) {
		set_material(material);
	}

	double get_intersection_distance(ray &r);
	aabb& get_aabb();
	vector3f get_normal(vector3f point);
};