#include "kdtree.hpp"

/* some predefined values */
const double cost_t = 15;			// cost of traverse:
const double cost_i = 20;			// cost of intersect
const int maximum_depth = 20;

/* tools */
enum event_type { plane_start = 0, plane_end = 1 };
struct plane_event {
	double plane;
	event_type e_type;
	bool operator<(plane_event &another) {
		if (plane == another.plane)
			return e_type < another.e_type;
		return plane < another.plane;
	}
};
int dim;
double cost_min;

/* statistics */
long long traverse_count;			// number of traversing
long long intersection_count;		// number of candidate intersection comparison


/* is leaf? */
bool kdtree::is_leaf() {
	return (left_kdtree == NULL);
}

/* is root? */
bool kdtree::is_root() {
	return (parent_kdtree == NULL);
}

aabb& kdtree::get_aabb() {
	return bbox;
}

/* get cost according to surface area */
double get_cost(double prob_left, double prob_right, int num_left, int num_right) 
{
	double cost_lambda = 1.0;
	if (num_left == 0 || num_right == 0)
		cost_lambda = 0.8;
	return cost_lambda * (cost_t + cost_i * (prob_left * num_left + prob_right * num_right));
}

/* get cost of sah(surface area heuristic) method for certain plane */
double get_sah_cost(double plane, aabb &voxel, int num_left, int num_right) 
{
	aabb voxel_left, voxel_right;
	voxel.split_aabb(dim, plane, voxel_left, voxel_right);
	double sa_left = voxel_left.get_surface_area();
	double sa_right = voxel_right.get_surface_area();
	double sa_union = voxel.get_surface_area();

	return get_cost(sa_left / sa_union, sa_right / sa_union, num_left, num_right);
}

/* get sorted (plane, event) list so that plane calculation can be accelerated */
void get_plane_event_list(vector<primitive*> pris, aabb voxel, vector<plane_event> &plane_event_list)
{
	for (int i = 0; i < pris.size(); i++)
	{
		primitive &pri = *(pris[i]);
		aabb clipped_box = aabb_intersection(pri.get_aabb(), voxel);
		plane_event start_event, end_event;
		start_event.plane = clipped_box.min[dim], start_event.e_type = plane_start;
		end_event.plane = clipped_box.max[dim], end_event.e_type = plane_end;
		plane_event_list.push_back(start_event);
		plane_event_list.push_back(end_event);
	}
	sort(plane_event_list.begin(), plane_event_list.end());
}

bool min_sort_function(primitive &first, primitive &second) {
	return first.get_aabb().min[dim] < second.get_aabb().min[dim];
}
bool max_sort_function(primitive &first, primitive &second) {
	return first.get_aabb().max[dim] < second.get_aabb().max[dim];
}

/* get best plane in current best_dim(d) using sah cost comparison */
void kdtree::get_best_plane(vector<primitive*> &pris, aabb &voxel)
{
	int num_left = 0, num_right = pris.size();

	vector<plane_event> plane_event_list;
	get_plane_event_list(pris, voxel, plane_event_list);

	for (int i = 0; i < plane_event_list.size(); i++)
	{
		double cost;
		plane_event &pe = plane_event_list[i];

		if (pe.plane == voxel.min[dim] || pe.plane == voxel.max[dim])
			continue;

		if (pe.e_type == plane_start)
			cost = get_sah_cost(pe.plane, voxel, num_left++, num_right);
		else if (pe.e_type == plane_end)
			cost = get_sah_cost(pe.plane, voxel, num_left, --num_right);

		if (cost < cost_min) {
			cost_min = cost;
			best_dim = dim;
			best_plane = pe.plane;
		}
	}
}

/* divide list of triangles(primitive) by given plane*/
void kdtree::classify_primitives(vector<primitive*> &pris, double plane, vector<primitive*> &left, vector<primitive*> &right) {
	for (int i = 0; i < pris.size(); i++)
	{
		if (pris[i]->get_aabb().min[best_dim] < plane) 
			left.push_back(pris[i]);
		if (pris[i]->get_aabb().max[best_dim] > plane)
			right.push_back(pris[i]);
		else if (pris[i]->get_aabb().min[best_dim] == plane)
			right.push_back(pris[i]);
		
	}
}

/* build kd-tree */
kdtree::kdtree(aabb &voxel, vector<primitive*> &pris, int depth, kdtree* parent)
{
	parent_kdtree = parent;
	left_kdtree = right_kdtree = NULL;
	bbox = voxel;

	/* end criteria; make this leaf */
	if (depth > maximum_depth || pris.size() < 1)
	{
		primitives = pris;
	}

	/* general case: determine whether and how to devide current voxel */
	else
	{
		double cost_asleaf = cost_i * pris.size();	
		cost_min = 1E100;							

		for (dim = 0; dim < 3; dim++)
		{
			get_best_plane(pris, voxel);
		}

		/* internal node; divide further */
		if (cost_min < cost_asleaf)
		{
			vector<primitive*> left_pris, right_pris;
			classify_primitives(pris, best_plane, left_pris, right_pris);

			aabb left_voxel, right_voxel;
			voxel.split_aabb(best_dim, best_plane, left_voxel, right_voxel);

			left_kdtree = new kdtree(left_voxel, left_pris, depth + 1, this);
			right_kdtree = new kdtree(right_voxel, right_pris, depth + 1, this);
		}

		/* leaf; division end */
		else							
		{
			primitives = pris;
		}
	}
}

/* locate the leaf in which given point exists */
kdtree* kdtree::find_leaf(vector3f &point) {
	if (is_leaf())
		return this;
	else if (point[best_dim] < best_plane) 
		return left_kdtree->find_leaf(point);
	else
		return right_kdtree->find_leaf(point);
}

/* re-locate the leaf if the point get out of the leaf */
kdtree* kdtree::backtrack_leaf(vector3f &point) {
	if (bbox.is_inside(point))
		return find_leaf(point);
	else if (is_root())
		return NULL;
	else
		return parent_kdtree->backtrack_leaf(point);
}


double get_enter_distance(aabb &voxel, ray &r) {
	double enter_distance = -1e100;
	for (int i = 0; i < 3; i++) {
		double dist;
		if (r.direction[i] > eps)
			dist = (voxel.min[i] - r.origin[i]) / r.direction[i];
		else if (r.direction[i] < -eps)
			dist = (voxel.max[i] - r.origin[i]) / r.direction[i];
		else 
			continue;

		if (dist > enter_distance)
			enter_distance = dist;
	}
	return enter_distance;
}

double get_exit_distance(aabb &voxel, ray &r) {
	double exit_distance = 1e100;
	for (int i = 0; i < 3; i++) {
		double dist;
		if (r.direction[i] > eps)
			dist = (voxel.max[i] - r.origin[i]) / r.direction[i];
		else if (r.direction[i] < -eps)
			dist = (voxel.min[i] - r.origin[i]) / r.direction[i];
		else
			continue;

		if (dist < exit_distance)
			exit_distance = dist;
	}
	return exit_distance;
}
/* kd-tree traversing */
primitive* kdtree::get_intersecting_primitive(ray &r)
{
	/* internal node should take care of subresult */
	if (!is_leaf()) 
	{
		kdtree* leaf = backtrack_leaf(r.origin);

		if (leaf == NULL)
			return NULL;
		else
			return leaf->get_intersecting_primitive(r);
	}

	/* leaf can only focus on determining intersections */
	else
	{
		double dist;
		for (int i = 0; i < primitives.size(); i++) {
			primitive &pri = *primitives[i];
			dist = pri.get_intersection_distance(r);
			if (dist > 0) {
				r.origin = r.origin + (dist - eps) * r.direction;
				return &pri;
			}
		}
		
		/* no intersection found */
		double exit_distance = get_exit_distance(bbox, r);
		r.origin = r.origin + (exit_distance + eps) * r.direction;

		return parent_kdtree->get_intersecting_primitive(r);
	}
}

primitive* get_intersecting_primitive(kdtree &root, ray &r) 
{
	/* ray origin is outside */
	if (!root.get_aabb().is_inside(r.origin))
	{
		double enter_distance = get_enter_distance(root.get_aabb(), r);
		double exit_distance = get_exit_distance(root.get_aabb(), r);

		if (enter_distance > exit_distance)
			return NULL;
		else
			r.origin = r.origin + (enter_distance + eps) * r.direction;
	}

	/* ray origin is in the get_aabb() */
	return root.get_intersecting_primitive(r);
}