#pragma once
#pragma pack(push, 1)
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#ifndef uint8_t
typedef unsigned char uint8_t;
#endif

#ifndef uint16_t
typedef unsigned short uint16_t;
#endif

#ifndef uint32_t
typedef unsigned int uint32_t;
#endif

using namespace std;

class rgb_pixel 
{
public:
	uint8_t blue;
	uint8_t green;
	uint8_t red;
	uint8_t alpha;

	rgb_pixel() {};
	rgb_pixel(uint32_t r, uint32_t g, uint32_t b) {
		red = r, green = g, blue = b, alpha = 0;
	}
	rgb_pixel(uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
		red = r, green = g, blue = b, alpha = a;
	}
	rgb_pixel(rgb_pixel &pix) {
		blue = pix.blue, red = pix.red, green = pix.green, alpha = pix.alpha;
	}

	rgb_pixel& operator=(rgb_pixel &pix) {
		blue = pix.blue, red = pix.red, green = pix.green, alpha = pix.alpha;
		return *this;
	}
};

class bmp
{
private:
	struct bmp_file_header
	{
		uint16_t type;
		uint32_t file_size;		/* the size of the BMP file in bytes */
		uint16_t creator1;		/* reserved. */
		uint16_t creator2;		/* reserved. */
		uint32_t offset;		/* the offset, i.e. starting address,
								of the byte where the bitmap data can be found. */
	};
	struct bmp_info_header {
		uint32_t header_size;	/* the size of this header (40 bytes) */
		uint32_t width;			/* the bitmap width in pixels */
		uint32_t height;		/* the bitmap height in pixels */
		uint16_t nplanes;		/* the number of color planes being used. Must be set to 1. */
		uint16_t depth;			/* the number of bits per pixel, which is the color depth of the image. */
		uint32_t compress_type; /* the compression method being used. */
		uint32_t image_size;	/* the image size. This is the size of the raw bitmap data */
		uint32_t hres;			/* the horizontal resolution of the image. */
		uint32_t vres;			/* the vertical resolution of the image. */
		uint32_t ncolors;		/* the number of colors in the color palette or 0 to default to 2<sup><i>n</i></sup>. */
		uint32_t nimpcolors;	/* the number of important colors used or 0 when every color is important. generally ignored. */
	};

	uint32_t width;
	uint32_t height;
	uint32_t depth;
	rgb_pixel *data;
	string file_name;

	friend ofstream& operator<< (ofstream& ofs, const bmp_file_header &bfh) {
		ofs.write(reinterpret_cast<const char*> (&bfh), sizeof(bmp_file_header));
		return ofs;
	}

	friend ofstream& operator<< (ofstream& ofs, const bmp_info_header &bih) {
		ofs.write(reinterpret_cast<const char*> (&bih), sizeof(bmp_info_header));
		return ofs;
	}

public:

	bmp(int w, int h, int d) {
		width = w;
		height = h;
		depth = d >> 3;
		data = new rgb_pixel[width * height];
		file_name = "";
	}

	~bmp() {
		delete data;
	}

	void save(const string &file_name) 
	{
		ofstream ofs(file_name.c_str(), ios::out);

		bmp_file_header bfh;
		bmp_info_header bih;

		bih.header_size		= sizeof (bmp_info_header);
		bih.width			= width;
		bih.height			= height;
		bih.nplanes			= 1;
		bih.depth			= static_cast<uint16_t> (depth << 3);		
		bih.compress_type	= 0;
		bih.image_size		= ((depth * width + 3) & 0xfffffffc) * height;
		bih.hres			= 3780;
		bih.vres			= 3780;
		bih.ncolors			= 0;
		bih.nimpcolors		= 0;

		bfh.type			= 19778;
		bfh.file_size		= sizeof (bmp_file_header) + sizeof (bmp_info_header) + bih.image_size + 1;
		bfh.creator1		= 0;
		bfh.creator2		= 0;
		bfh.offset			= sizeof (bmp_file_header) + sizeof (bmp_info_header);

		ofs << bfh << bih;
		

		ofs.write(reinterpret_cast<char*> (data),  bih.image_size);

		ofs.close();
	}

	rgb_pixel& operator() (uint32_t x, uint32_t y) {
		return data[x + y * width];
	}
};