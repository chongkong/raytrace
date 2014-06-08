#include "primitive.hpp"

ray::ray(vector3f orig, vector3f dir) {
	origin = orig, direction = dir.normal();
}

aabb::aabb() {

}
aabb::aabb(double minx, double miny, double minz, double maxx, double maxy, double maxz) {
	min.vec[0] = minx, min.vec[1] = miny, min.vec[2] = minz;
	max.vec[0] = maxx, max.vec[1] = maxy, max.vec[2] = maxz;
}
aabb::aabb(vector3f minimum, vector3f maximum) {
	min = minimum, max = maximum;
}

aabb& aabb::operator=(const aabb &another) {
	min = another.min, max = another.max;
	return *this;
}

bool aabb::is_inside(vector3f point) const{
	return
		(min.vec[0] <= point.vec[0] && point.vec[0] <= max.vec[0])
		&& (min.vec[1] <= point.vec[1] && point.vec[1] <= max.vec[1])
		&& (min.vec[2] <= point.vec[2] && point.vec[2] <= max.vec[2]);
}

void aabb::split_aabb(int dimension, double plane, aabb &result_left, aabb &result_right) const {
	result_left = result_right = *this;
	result_left.max.vec[dimension] = plane;
	result_right.min.vec[dimension] = plane;
}

double aabb::get_surface_area() const {
	double a = max.vec[0] - min.vec[0];
	double b = max.vec[1] - min.vec[1];
	double c = max.vec[2] - min.vec[2];
	return 2 * ((a + b) * c + a * b);
}

aabb aabb_intersection(const aabb &first, const aabb &second) {
	double minx = max(first.min.vec[0], second.min.vec[0]);
	double miny = max(first.min.vec[1], second.min.vec[1]);
	double minz = max(first.min.vec[2], second.min.vec[2]);
	double maxx = min(first.max.vec[0], second.max.vec[0]);
	double maxy = min(first.max.vec[1], second.max.vec[1]);
	double maxz = min(first.max.vec[2], second.max.vec[2]);

	return aabb(minx, miny, minz, maxx, maxy, maxz);
}


inline double min3(double a, double b, double c) {
	return min(a, min(b, c));
}
inline double max3(double a, double b, double c) {
	return max(a, max(b, c));
}

triangle::triangle() {

}
triangle::triangle(vector3f va, vector3f vb, vector3f vc) {
	vertex.push_back(va);
	vertex.push_back(vb);
	vertex.push_back(vc);

	/* intersection acceleration (ta: triangle acceleration) */
	ta.normal = ((vb - va) ^ (vc - vb)).normal();
	ta.distance = va * ta.normal;
	vector3f a = vc - va, b = vb - va;
	int axis = (ta.normal[0] > 0.57) ? 0 : (ta.normal[1] > 0.57) ? 1 : 2;
	ta.u = (axis + 1) % 3;
	ta.v = (axis + 2) % 3;
	ta.anu = a[ta.u] / (a[ta.u] * b[ta.v] - a[ta.v] * b[ta.u]);
	ta.anv = a[ta.v] / (a[ta.u] * b[ta.v] - a[ta.v] * b[ta.u]);
	ta.bnu = b[ta.u] / (b[ta.u] * a[ta.v] - b[ta.v] * a[ta.u]);
	ta.bnv = b[ta.v] / (b[ta.u] * a[ta.v] - b[ta.v] * a[ta.u]);

	// kdtree structure
	for (int i = 0; i < 3; i++) {
		bbox.min[i] = min3(va[i], vb[i], vc[i]) - eps2;
		bbox.max[i] = max3(va[i], vb[i], vc[i]) + eps2;
	}
}

double triangle::get_intersection_distance(ray &r) 
{
	double dist = (ta.distance - (ta.normal * r.origin)) / (ta.normal * r.direction);
	double pu = r.origin[ta.u] + dist * r.direction[ta.u] - vertex[0][ta.u];
	double pv = r.origin[ta.v] + dist * r.direction[ta.v] - vertex[0][ta.v];

	double a = ta.bnu * pv - ta.bnv * pu;
	if (a < 0)
	{
		return -1;
	}

	double b = ta.anu * pv - ta.anv * pu;
	if (b < 0 || a + b > 1)
	{
		return -1;
	}
	else
	{
		return dist;
	}
}

aabb& triangle::get_aabb() 
{
	return bbox;
}

vector3f triangle::get_normal(vector3f point)
{
	
	if (!vertex_normal.empty())
	{
		double pu = point[ta.u], pv = point[ta.v];
		double a = ta.bnu * pv - ta.bnv * pu;
		double b = ta.anu * pv - ta.anv * pu;

		return (1 - a - b) * vertex_normal[0] + b * vertex_normal[1] + a * vertex_normal[2];
	}
	else
	{
		return ta.normal;
	}
}

ball::ball(vector3f c, double r) 
{
	center = c;
	radius = r;

	r = r + eps2;
	bbox = aabb(c - vector3f(r, r, r), c + vector3f(r, r, r));
}

double ball::get_intersection_distance(ray &r)
{
	vector3f dp = center - r.origin;
	double b = r.direction * dp;
	double c = dp*dp - radius*radius;
	double d = b*b - c;

	if (d < eps)
	{
		return -1;
	}
	else
	{
		double rd = sqrt(d);
		if (b + rd < 0)
		{
			return -1;
		}
		else if (b - rd < 0)
		{
			return b + rd;
		}
		else
		{
			return b - rd;
		}
	}
}

aabb& ball::get_aabb()
{
	return bbox;
}

vector3f ball::get_normal(vector3f point)
{
	return (point - center).normal();
}