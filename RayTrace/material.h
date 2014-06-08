#pragma once
#include "vector3f.hpp"
#include "bitmap_image.hpp"


enum MATERIAL{
	emerald = 0,
	jade = 1,
	obsidian = 2,
	pearl = 3,
	ruby = 4,
	turquoise = 5,
	brass = 6,
	bronze = 7,
	chrome = 8,
	copper = 9,
	gold = 10,
	silver = 11,
	pewter = 12,
	polished_bronze = 13,
	polished_copper = 14,
	polished_gold = 15,
	polished_silver = 16,
	black_plastic = 17,
	cyan_plastic = 18,
	green_plastic = 19,
	red_plastic = 20,
	white_plastic = 21,
	yellow_plastic = 22,
	black_rubber = 23,
	cyan_rubber = 24,
	green_rubber = 25,
	red_rubber = 26,
	white_rubber = 27,
	yellow_rubber = 28,
	glass = 29
};


class rgba {
public:
	double vec[4];
	rgba(double red, double green, double blue, double alpha) {
		vec[0] = red, vec[1] = green, vec[2] = blue, vec[3] = alpha;
	}
	rgba(double *color) {
		vec[0] = color[0], vec[1] = color[1], vec[2] = color[2], vec[3] = color[3];
	}
	rgba(rgba& another) {
		for (int i = 0; i < 4; i++)
			vec[i] = another[i];
	}
	rgba& operator=(rgba &another) {
		for (int i = 0; i < 4; i++) 
			vec[i] = another[i];
		return *this;
	}
	double& operator[](int index) {
		return vec[index];
	}
};

class phong {
public:
	virtual rgba& get_ambient(double u, double v) = 0;
	virtual rgba& get_diffuse(double u, double v) = 0;
	virtual rgba& get_specular(double u, double v) = 0;
	virtual rgba& get_emission(double u, double v) = 0;
};

class material : public phong {
private:
	rgba amb, dif, spc, ems;
public:
	material(rgba &ambient, rgba &diffuse, rgba &specular, rgba &emission);
	material(MATERIAL material);

	// doesn't need u-v coordinate
	rgba& get_ambient(double u = 0, double v = 0) { return amb; }
	rgba& get_diffuse(double u = 0, double v = 0) { return dif; }
	rgba& get_specular(double u = 0, double v = 0) { return spc; }
	rgba& get_emission(double u = 0, double v = 0) { return ems; }

	bool is_transparent();
};

class texture : public phong {
private:
	bitmap_image img;
	double alp;
	double amb;
public:
	texture(bitmap_image &image) : alp(1.0), amb(0.1) {
		img = image;
	}
	texture(bitmap_image &image, double alpha) : amb(0.1) {
		img = image;
		alp = alpha;
	}
	texture(bitmap_image &image, double alpha, double ambient) {
		img = image;
		alp = alpha;
		amb = ambient;
	}

	// only ambient and diffuse mapping
	rgba& get_ambient(double u, double v) {
		
		unsigned imgx = u * img.width, imgy = v * img.height;
		unsigned char red, green, blue;
		img.get_pixel(imgx, imgy, red, green, blue);
		return rgba(amb * red / 255.0, amb * green / 255.0, amb * blue / 255.0, alp);
	}
	rgba& get_diffuse(double u, double v) {
		unsigned imgx = u * img.width, imgy = v * img.height;
		unsigned char red, green, blue;
		img.get_pixel(imgx, imgy, red, green, blue);
		return rgba(red / 255.0, green / 255.0, blue / 255.0, alp);
	}

	// no specular nor emission
	rgba& get_specular(double u = 0, double v = 0) {
		return rgba(0.0, 0.0, 0.0, 1.0);
	}
	rgba& get_emission(double u = 0, double v = 0) {
		return rgba(0.0, 0.0, 0.0, 1.0);
	}

};