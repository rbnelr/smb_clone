#define _CRT_SECURE_NO_WARNINGS 1

#include "stdio.h"
//
#define STB_IMAGE_IMPLEMENTATION 1
#define STB_IMAGE_STATIC 1
//#define STBI_ONLY_BMP	1
#define STBI_ONLY_PNG	1
//#define STBI_ONLY_TGA	1
//#define STBI_ONLY_JPEG	1
//#define STBI_ONLY_HDR	1

#include "stb_image.h"
//
#define STB_IMAGE_WRITE_IMPLEMENTATION 1

#include "stb_image_write.h"
//
#include "raz_libs/vector.hpp"

typedef s32v2 iv2;
typedef u8v4 icol;

struct Image {
	
	cstr	name; // filename, or just for debugging
	
	iv2		dim;
	int		pixel_size; // 3 or 4
	
	uptr	stride;
	uptr	size;
	
	u8*		pixels; // has to be allocated with malloc
	
	bool stb_load () {
		dim = 0;
		pixels = 0;
		stride = 0;
		size = 0;
		
		pixels = stbi_load(name, &dim.x,&dim.y, &pixel_size, 0);
		if (!pixels) {
			fprintf(stderr, "File \"%s\" could not be loaded!\n", name);
			return false;
		}
		
		stride = (uptr)dim.x * pixel_size;
		size = stride * dim.y;
		
		return true;
	}
	
	void allocate () {
		if (any(dim == 0)) return;
		
		stride = (uptr)dim.x * pixel_size;
		size = stride * dim.y;
		
		pixels = (u8*)malloc(size);
	}
	~Image () {
		if (pixels) free(pixels);
	}
	
	bool stb_overwrite () { // always writes png
		int ret = stbi_write_png(name, dim.x,dim.y, pixel_size, pixels, (int)stride);
		
		return ret != 0;
	}
	
	u8 const* at (int x, int y) const {
		return pixels +(y * stride + x * pixel_size);
	}
	u8 const* at (iv2 pos) const {
		return at(pos.x, pos.y);
	}
	
	u8* at (int x, int y) {
		return pixels +(y * stride + x * pixel_size);
	}
	u8* at (iv2 pos) {
		return at(pos.x, pos.y);
	}
	
	void clear (u8 r, u8 g, u8 b, u8 a) {
		
		u8 col[4] = { r,g,b,a };
		
		for (int y=0; y<dim.y; ++y) {
			for (int x=0; x<dim.x; ++x) {
				for (int ch=0; ch<pixel_size; ++ch) {
					at(x, y)[ch] = col[ch];
				}
			}
		}
	}
};

int main () {
	
	Image in;
	in.name = "C:\\ssd_coding\\image_process\\image_process\\mario.png";
	
	if (!in.stb_load()) return 1;
	
	Image out;
	out.name = "C:\\ssd_coding\\image_process\\image_process\\mario_correct.png";
	
	out.dim.x = in.dim.x / 17 * 16;
	out.dim.y = in.dim.y;
	out.pixel_size = in.pixel_size;
	
	out.allocate();
	
	//out.clear(255,0,255,255);
	
	for (int sprite=0; sprite<in.dim.x / 17; ++sprite) {
		for (int y=0; y<in.dim.y; ++y) {
			for (int x=0; x<16; ++x) {
				for (int ch=0; ch<in.pixel_size; ++ch) {
					out.at(sprite*16 +x, y)[ch] = in.at(sprite*17 +x, y)[ch];
					//out.at(sprite*16 +x, y)[ch] = sprite % 2 ? 255 : 0;
				}
			}
		}
	}
	
	if (!out.stb_overwrite()) return 1;
	
	return 0;
}
