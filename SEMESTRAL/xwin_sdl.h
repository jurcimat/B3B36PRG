/*
 * File name: xwin_sdl.h
 * Date:      2015/06/18 14:37
 * Author:    Jan Faigl
 */

#ifndef __XWIN_SDL_H__
#define __XWIN_SDL_H__

int xwin_init(int w, int h);
void xwin_close();
void xwin_redraw(int w, int h, unsigned char *img);
void xwin_poll_events(void);

#endif

/* end of xwin_sdl.h */
