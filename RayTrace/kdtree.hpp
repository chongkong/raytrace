#pragma once
#include <vector>
#include <algorithm>
#include "vector3f.hpp"
#include "primitive.hpp"

class kdtree 
{
private:
	int best_dim;
	double best_plane;
	aabb bbox;

	bool is_leaf();
	bool is_root();
	void get_best_plane(vector<primitive*> &tris, aabb &voxel);
	void classify_primitives(vector<primitive*> &tris, double plane, vector<primitive*> &left, vector<primitive*> &right);
	kdtree* find_leaf(vector3f &point);
	kdtree* backtrack_leaf(vector3f &point);

public:
	vector<primitive*> primitives;
	kdtree *parent_kdtree, *left_kdtree, *right_kdtree;

	kdtree(aabb &voxel, vector<primitive*> &tris, int depth = 0, kdtree* parent = NULL);
	aabb& get_aabb();
	primitive* get_intersecting_primitive(ray &r);
};

primitive* get_intersecting_primitive(kdtree &root, ray &r);