#ifndef ALPHAUTILS_H
#define ALPHAUTILS_H

#include "imgui.h"
#include <SDL.h>
//  Criteria::Topleft, means alpha value is whatever color is on top left pixel (0,0) in graphics
//	Criteria::Alphavalue, means use alpha value in 32 bit pixel values
//	Criteria::Magenta, means alpha value is full magenta, R=255, G=0, B=255,
//	Criteria::anycolor, means alpha value is any color, R=?, G=?, B=?

// in code:  void SomeFunction(bool* open, char* buffer, int bufferSize, Criteria crit = Criteria::topleft ) {
enum class Criteria {
	topleft,
	alpha,
	magenta,
	anycolor
};
//void putpixel(SDL_Surface *surf, int px, int py, Uint32 color);
int getpixel(SDL_Surface *surf, int px, int py);
int getAlphaValue(SDL_Surface *surf, Uint32 pixelvalue, Criteria criteria, Uint32 selectedAlphaColor);
bool isAlphaTransparent(int alfa);
SDL_Surface* createNewSurfaceRGBA(int width, int height);
//unsigned int getGreyColor1(float& tickerTimer);
//



unsigned int getTimedSinus();
unsigned int getTimedSinus90();
unsigned int getTimedSinus180();
unsigned int  getTimedBinary();


#endif








