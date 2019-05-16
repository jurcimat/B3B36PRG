#include <stdio.h>
#include <stdlib.h>

#include "window.h"
#include "xwin_sdl.h"


image* window_init(int width, int height, uint8_t chunk_size_re, uint8_t chunk_size_im){
	image* img = (image*)malloc(sizeof(image));
	pixel* data = (pixel*)malloc(width*height*sizeof(pixel));
	img->nbr_of_chunks = (width*height)/(chunk_size_re * chunk_size_im);
	img->data = data;
	img->width = width;
	img->height = height;
	xwin_init(width, height);
	return img;
}
	
void window_refresh(image* img){
	//fprintf(stderr, "inside window refresh\r\n");
	xwin_redraw(img->width, img->height, (unsigned char*)img->data);
}

void close_image(void){
	
	xwin_close();
}

void free_image(image* img){
	free(img->data);
	free(img);
	img = NULL;
}

image* window_black_screen(image* img){
	for (int x = 0; x < (img->height * img->width); x++){
		img->data[x] = (pixel){.R = 0, .G = 0, .B = 0};  	
	}
	window_refresh(img);
	return img;
}

image* set_pixel(image* img, uint8_t cid, int x, int y, uint8_t R, uint8_t G, uint8_t B){
	if (img == NULL){
		return NULL;
	}
	if (cid < 0 || cid > img->nbr_of_chunks){
		fprintf(stderr, "ERROR: Chunk ID is out of range\n\r");	
	}
	if (x < 0 || x > img->width || y < 0 || y > img->height){
		//fprintf(stderr, "x = %d y = %d width = %d height = %d\n\r",x,y,img->width,img->height);
		fprintf(stderr, "ERROR: Set pixel input has coordinates out of range\n\r");
		return NULL;				
	}
	img->data[y*(img->width) + x].R = R;
	img->data[y*(img->width) + x].G = G;
	img->data[y*(img->width) + x].B = B;
	return img;
}

image* set_pixel_by_iter(image* img, uint8_t cid, int x, int y, uint8_t iter, uint8_t n){
	double t = (double)iter/(double)n;
	double R = 9 * (1 - t) * t * t * t * 255;
	double G = 15 * (1 - t) * (1 - t) * t * t * 255;
	double B = 8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255;	
	return set_pixel(img, cid, x, y, (uint8_t)R, (uint8_t)G, (uint8_t)B);
}


/* end of window.c */
