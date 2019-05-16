

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <stdint.h>
#include <stdbool.h>
#include "xwin_sdl.h"


#define RGB_MAX 255


typedef struct {
	uint8_t R;
	uint8_t G;
	uint8_t B;
} pixel;

typedef struct {
	uint8_t cid;
	uint8_t chunk_width;
	uint8_t chunk_height;
	pixel* data;
} chunk;

typedef struct {
	int nbr_of_chunks;
	int height;
	int width;
	pixel* data;
} image;

image* window_init(int width, int height, uint8_t chunk_size_re, uint8_t chunk_size_im);
void window_refresh(image* img);
image* window_black_screen(image* img);
image* set_pixel(image* img, uint8_t cid, int x, int y, uint8_t R, uint8_t G, uint8_t B);
image* set_pixel_by_iter(image* img, uint8_t cid, int x, int y, uint8_t iter, uint8_t n);
void free_image(image* img);
void close_image(void);




#endif

/* end of window.h */
