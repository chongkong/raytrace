#include "bsptree.h"

const double epsilon = 1E-9;

bsptree::bsptree() {
	// nothing
}

bsptree::bsptree(list<polygon> polygons)
{
	polygon root = polygons.front();
	forward_list.push_back(root);
	polygons.pop_front();

	plane_normal = root.normal;
	plane_distance = plane_normal * root.vertex[0];

	list<polygon> front_bsp_list;
	list<polygon> back_bsp_list;

	for (polygon poly : polygons)
	{
		// case 1: coincident
		if (abs(distance(poly.vertex[0])) < epsilon)
		{
			if ((poly.normal - plane_normal).norm() < epsilon) {
				forward_list.push_back(poly);
				continue;
			}
			else if ((poly.normal + plane_normal).norm() < epsilon) {
				backward_list.push_back(poly);
				continue;
			}
		}

		int flag = 3;
		for (unsigned int i = 0; i < poly.vertex.size(); i++) {
			if (distance(poly.vertex[i]) > epsilon)
				flag &= 1;
			else if (distance(poly.vertex[i]) < -epsilon)
				flag &= 2;
		}

		// case 2: in front of
		if (flag & 1)
		{
			front_bsp_list.push_back(poly);
			continue;
		}
			
		// case 3: in back of
		else if (flag & 2)
		{
			back_bsp_list.push_back(poly);
			continue;
		}

		// case 4: spanning
		else if (flag == 0)
		{
			polygon front, back;
			split_polygon(poly, front, back);
			front_bsp_list.push_back(front);
			back_bsp_list.push_back(back);
			continue;
		}
	}

	front_bsp = back_bsp = NULL;
	size = 1;
	if (!front_bsp_list.empty())
	{
		front_bsp = new bsptree(front_bsp_list);
		size += front_bsp->size;
	}
	if (!back_bsp_list.empty())
	{
		back_bsp = new bsptree(back_bsp_list);
		size += back_bsp->size;
	}

}

double bsptree::distance(vector3f v) {
	return (plane_normal * v) - plane_distance;
}

void bsptree::split_polygon(polygon &poly, polygon &front, polygon &back)
{
	for (unsigned int i = 0; i < poly.vertex.size(); i++) 
	{
		// case 1: vertex itself is a intersection point
		if (abs(distance(poly.vertex[i])) < epsilon) {
			front.vertex.push_back(poly.vertex[i]);
			back.vertex.push_back(poly.vertex[i]);
		}
			
		// case 2: first vertex on the front, next vertex on the back
		else if (distance(poly.vertex[i]) > 0) 
		{
			front.vertex.push_back(poly.vertex[i]);

			if (distance(poly.vertex[(i + 1) % poly.vertex.size()]) < 0) 
			{
				vector3f intersection_point = poly.vertex[i];
				vector3f begin_point = poly.vertex[i];
				vector3f end_point = poly.vertex[(i + 1) % poly.vertex.size()];
				while (abs(distance(intersection_point)) > epsilon) {
					if (distance(intersection_point) > 0) {
						begin_point = intersection_point;
						intersection_point = (begin_point + end_point) / 2;
					}
					else {
						end_point = intersection_point;
						intersection_point = (begin_point + end_point) / 2;
					}
				}
				front.vertex.push_back(intersection_point);
				back.vertex.push_back(intersection_point);
			}
		}

		// case 3: first vertex on the back, next vertex on the front
		else
		{
			back.vertex.push_back(poly.vertex[i]);

			if (distance(poly.vertex[(i + 1) % poly.vertex.size()]) > 0) 
			{
				vector3f intersection_point = poly.vertex[i];
				vector3f begin_point = poly.vertex[i];
				vector3f end_point = poly.vertex[(i + 1) % poly.vertex.size()];
				while (abs(distance(intersection_point)) > epsilon) {
					if (distance(intersection_point) < 0) {
						begin_point = intersection_point;
						intersection_point = (begin_point + end_point) / 2;
					}
					else {
						end_point = intersection_point;
						intersection_point = (begin_point + end_point) / 2;
					}
				}
				front.vertex.push_back(intersection_point);
				back.vertex.push_back(intersection_point);
			}
		}
	}

	front.normal = back.normal = poly.normal;
}
