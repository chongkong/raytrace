#include "bmp.h"

const uint32_t default_dpi_x = 3780;
const uint32_t default_dpi_y = 3780;

typedef struct {
	uint32_t filesize;		/* the size of the BMP file in bytes */
	uint16_t creator1;		/* reserved. */
	uint16_t creator2;		/* reserved. */
	uint32_t offset;		/* the offset, i.e. starting address,
							of the byte where the bitmap data can be found. */
} bmp_file_header_t;

typedef struct {
	uint32_t header_size;	/* the size of this header (40 bytes) */
	uint32_t width;			/* the bitmap width in pixels */
	uint32_t height;		/* the bitmap height in pixels */
	uint16_t nplanes;		/* the number of color planes being used. Must be set to 1. */
	uint16_t depth;			/* the number of bits per pixel, which is the color depth of the image. */
	uint32_t compress_type; /* the compression method being used. */
	uint32_t bmp_byte_size;	/* the image size. This is the size of the raw bitmap data */
	uint32_t hres;			/* the horizontal resolution of the image. */
	uint32_t vres;			/* the vertical resolution of the image. */
	uint32_t ncolors;		/* the number of colors in the color palette or 0 to default to 2<sup><i>n</i></sup>. */
	uint32_t nimpcolors;	/* the number of important colors used or 0 when every color is important. generally ignored. */
} bmp_info_header_t;

typedef struct {
	uint8_t magic[2];		/* the magic number used to identify the BMP file:
							0x42 0x4D (Hex code points for B and M).
							The following entries are possible:
							BM - Windows 3.1x, 95, NT, ... etc
							BA - OS/2 Bitmap Array
							CI - OS/2 Color Icon
							CP - OS/2 Color Pointer
							IC - OS/2 Icon
							PT - OS/2 Pointer. */
	bmp_file_header_t header;
	bmp_info_header_t dib;

	rgb_pixel_t **pixels;
	rgb_pixel_t *colors;
} bmp_t;

bmp_t
bmp_create (uint32_t width, uint32_t height, uint32_t depth)
{
	bmp_t *result = new bmp_t;

	result->magic[0] = 'B';
	result->magic[1] = 'M';

	result->dib.header_size = 40;
	result->dib.width = width;
	result->dib.height = height;
	result->dib.nplanes = 1;
	result->dib.depth = depth;
	result->dib.compress_type = 0;
	result->dib.bmp_byte_size =
	result->dib.hres = default_dpi_x;
	result->dib.vres = default_dpi_y;

	bmp_malloc_pixels(result);
	bmp_malloc_colors(result);

	return result;
}