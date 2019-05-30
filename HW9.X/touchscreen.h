/* 
 * File:   touchscreen.h
 * Author: erkin
 *
 * Created on May 23, 2019, 11:08 PM
 */

#ifndef touchscreen_H__
#define touchscreen_H__

//HW9 added functions

void read_XPT2046(unsigned short *x, unsigned short *y, unsigned int *z);
unsigned short x_Pixel(unsigned short x);
unsigned short y_Pixel(unsigned short y);
void buttons();
signed int push_button(unsigned short xPixel, unsigned short yPixel);

#endif

