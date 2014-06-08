#pragma once
#include <list>
#include "polygon.hpp"
#include "vector3f.h"

using namespace std;

class bsptree
{
public:
	vector3f plane_normal;
	double plane_distance;
	int size;
	list<polygon> forward_list;
	list<polygon> backward_list;
	bsptree *front_bsp;
	bsptree *back_bsp;
	
	bsptree();
	bsptree(list<polygon> polygons);

private:
	double distance(vector3f v);
	void split_polygon(polygon &poly, polygon &front, polygon &back);
};
