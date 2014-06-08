#include "ray_trace.h"

const int max_depth = 10;
const int shadow_sampling = 16;
const double distance_factor = 28.0;
const double area_size = 8.0;
const double n_t = 1.05;

vector3f point_light_src(5, 20, 20);
vector3f area_light_src(3, 20, 18);
vector3f background(0, 0, 0);

vector3f& get_light_coordinate() {
	return point_light_src;
}


vector3f& get_light_coordinate(int i, int j) {
	double randi = (i + (rand() & 63) / 63.0) * (area_size / shadow_sampling);
	double randj = (j + (rand() & 63) / 63.0) * (area_size / shadow_sampling);
	return area_light_src + vector3f(randi, 0, randj);
}

vector3f rgb_trim(vector3f& rgb) {
	vector3f ret;
	for (int i = 0; i < 3; i++) {
		if (rgb[i] > 1) ret[i] = 1;
		else if (rgb[i] < 0) ret[i] = 0;
		else ret[i] = rgb[i];
	}
	return ret;
}

inline vector3f get_ambient(double *amb) {
	return vector3f(amb[0], amb[1], amb[2]) * amb[3] + background * (1 - amb[3]);
}

inline vector3f get_specular(double *spc, primitive *pri, ray &r, kdtree &kdtree, int depth) {
	vector3f specular(0.0, 0.0, 0.0);
	if (spc[0] + spc[1] + spc[2] > eps)
	{
		vector3f surface_normal(pri->get_normal(r.origin));
		ray specular_ray(r.origin, r.direction - 2 * (r.direction * surface_normal) * surface_normal);
		specular = map_mul(get_ray_trace(specular_ray, kdtree, depth + 1), vector3f(spc));
	}
	return specular;
}

vector3f get_light_distribution(primitive *pri, ray &r, kdtree &kdtree) 
{
	// distribution
	vector3f distribution(0.0, 0.0, 0.0);
	double delta = 1.0 / (shadow_sampling * shadow_sampling);

	for (int i = 0; i < shadow_sampling; i++) {
		for (int j = 0; j < shadow_sampling; j++) {
			vector3f light_pos = get_light_coordinate(i, j);
			vector3f shadow_dir = light_pos - r.origin;
			ray shadow_ray(r.origin, shadow_dir);
			primitive *shadow_pri = get_intersecting_primitive(kdtree, shadow_ray);

			if (shadow_pri == NULL || (shadow_ray.origin - r.origin).norm() > shadow_dir.norm()) {
				// clear
				double dist = shadow_dir.norm() / distance_factor;
				double phong = pri->get_normal(r.origin) * shadow_ray.direction;
				distribution += delta * max(0.0, phong) / (dist * dist);
			}
			else {
				double occupacy = shadow_pri->get_material().dif[3];

				if (occupacy < 1 - eps2) 
				{
					// approximated fraction light
					double cos_value = shadow_pri->get_normal(shadow_ray.origin) * shadow_ray.direction;
					double fraction_value = (1 - cos_value) * (1 - cos_value) / 4.0;
					double phong = pri->get_normal(r.origin) * shadow_ray.direction;
					double dist = shadow_dir.norm() / distance_factor;
					distribution += delta * max(0.0, phong) * fraction_value * (1 - occupacy) / (dist * dist);
				
				}

				else 
				{
					// nothing
				}
			}
		}
	}

	return distribution;
}


vector3f get_diffuse(double *dif, primitive *pri, ray &r, kdtree &kdtree, int depth, double &spc_alpha)
{
	vector3f diffuse(0.0, 0.0, 0.0);
	if (pri->get_material().is_transparent())
	{
		vector3f surface_normal = pri->get_normal(r.origin);
		double cos_value = surface_normal * r.direction;
		double sin_value = sqrt(1 - cos_value * cos_value);
		ray fraction_ray(r.origin + 5 * eps2 * r.direction, r.direction);

		double cos_value_pow = (1 - abs(cos_value)) * (1 - abs(cos_value));
		double normal_t = (cos_value > 0) ? (1.0 / n_t) : n_t;
		double r0 = (normal_t - 1) / (normal_t + 1);
		r0 = r0 * r0;
		spc_alpha = r0 + (1 - r0) * cos_value_pow;

		// entering slow material
		if (cos_value < 0)
		{
			if (abs(sin_value) > eps) {
				vector3f base = (r.direction + (surface_normal * abs(cos_value))).normal();
				double sin_frac = sin_value / n_t;
				double cos_frac = sqrt(1 - sin_frac * sin_frac);
				fraction_ray.direction = ((sin_frac * base) - (cos_frac * surface_normal)).normal();
			}
			diffuse = dif[3] * map_mul(vector3f(dif), get_light_distribution(pri, r, kdtree))
				+ (1 - dif[3]) * get_ray_trace(fraction_ray, kdtree, depth + 1);
		}
		
		// exit slow material
		else
		{
			if (abs(sin_value) > eps) {
				vector3f base = (r.direction - (surface_normal * abs(cos_value))).normal();
				double sin_frac = sin_value  * n_t;
				if (sin_frac > 1.0) {		//total reflection
					spc_alpha += diffuse[3];
					return diffuse;
				}
				double cos_frac = sqrt(1 - sin_frac * sin_frac);
				fraction_ray.direction = ((sin_frac * base) + (cos_frac * surface_normal)).normal();
			}
			diffuse = dif[3] * map_mul(vector3f(dif), get_light_distribution(pri, r, kdtree))
				+ (1 - dif[3]) * get_ray_trace(fraction_ray, kdtree, depth + 1);
		}
	}
	else
	{
		diffuse = map_mul(vector3f(dif), get_light_distribution(pri, r, kdtree));
	}
	return diffuse;
}

vector3f get_ray_trace(ray &r, kdtree &kdtree, int depth) 
{
	if (depth >= max_depth)
	{
		return vector3f(0.0, 0.0, 0.0);
	}

	vector3f origin_cp = r.origin;
	primitive *pri = get_intersecting_primitive(kdtree, r);
	
	if (pri == NULL) 
	{
		return background;
	}

	else
	{
		double *amb = pri->get_material().amb;
		double *dif = pri->get_material().dif;
		double *spc = pri->get_material().spc;

		// ambient calculation
		vector3f ambient = get_ambient(amb);

		// specular calculation (no transparency)
		vector3f specular = get_specular(spc, pri, r, kdtree, depth);

		// diffuse calculation
		double spc_alpha = 1;
		vector3f diffuse = get_diffuse(dif, pri, r, kdtree, depth, spc_alpha);

		return rgb_trim(ambient + specular * spc_alpha + diffuse);
	}
}