
#include "AlphaUtils.h"
#include <math.h>

bool flipout = false;
bool flippy = false;


// from pixel value to alpha value
//  Criteria::Topleft, means alpha value is whatever color is on top left pixel (0,0) in graphics
//	Criteria::Magenta, means alpha value is full magenta, R=255, G=0, B=255,
//	Criteria::Alphavalue, means use alpha value in 32 bit pixel values
//	Criteria::anycolor, means alpha value is any color, R=?, G=?, B=?
// in code:  void SowmeFunction(bool* open, char* buffer, int bufferSize, Criteria crit = Criteria::topleft ) {
//
// examine pixelvalue and surface, using Criteria to select alpha value
// returns only 0 or 255, 0 = transparent, 255 = opaque.
int getAlphaValue(SDL_Surface *surf, Uint32 pixelvalue, Criteria criteria, Uint32 selectedAlphaColor) {
	SDL_PixelFormat *fmt = surf->format;
	Uint8 ru=0, gu=0, bu=0;
	if(criteria == Criteria::anycolor)
		SDL_GetRGB(selectedAlphaColor, fmt, &ru, &gu, &bu);
	Uint32 temp = 0;
	Uint8 red, green, blue, alpha;
	/* Get Red component */
	temp = pixelvalue & fmt->Rmask; /* Isolate red component */
	temp = temp >> fmt->Rshift;/* Shift it down to 8-bit */
	temp = temp << fmt->Rloss; /* Expand to a full 8-bit number */
	red = (Uint8)temp;
	/* Get Green component */
	temp = pixelvalue & fmt->Gmask; /* Isolate green component */
	temp = temp >> fmt->Gshift;/* Shift it down to 8-bit */
	temp = temp << fmt->Gloss; /* Expand to a full 8-bit number */
	green = (Uint8)temp;
	/* Get Blue component */
	temp = pixelvalue & fmt->Bmask; /* Isolate blue component */
	temp = temp >> fmt->Bshift;/* Shift it down to 8-bit */
	temp = temp << fmt->Bloss; /* Expand to a full 8-bit number */
	blue = (Uint8)temp;
	// Get Alpha component 
	temp = pixelvalue & fmt->Amask; /* Isolate alpha component */
	temp = temp >> fmt->Ashift;		/* Shift it down to 8-bit */
	temp = temp << fmt->Aloss;		/* Expand to a full 8-bit number */
	alpha = (Uint8)temp;
	if (criteria == Criteria::alpha) {	// ALPHA value in graphic
		if (alpha < 127) { //
			return 0;	// transparency
		}
		else {
			return 255;	// opaque
		}
	}
	if (criteria == Criteria::magenta) {	// Full MAGENTA 
		if (red == 255 && green == 0 && blue == 255) { // on magenta color, select 0 (full background)
			return 0;	// transparency
		}
		else {
			return 255;	// opaque
		}
	}
	if (criteria == Criteria::topleft) {	// top left pixel value is used as alpha
		int *mypixel = (int *)surf->pixels;
		Uint32 chosen_color = mypixel[0] & ~fmt->Amask;
		if ((pixelvalue & ~fmt->Amask) == chosen_color) {
			return 0;	// transparency
		}
		else {
			return 255;	// opaque
		}
	}
	if (criteria == Criteria::anycolor) {	// any color (r=?,g=?,b=?), not ready yet
		// DONE, convert selectedAlphaColor into RGB
		if (red == ru && green == gu && blue == bu) { // on selected color, select 0 (full background)
			return 0;	// transparency
		}
		else {
			return 255;	// opaque
		}
	}
	return alpha;
}
/*
void putpixel(SDL_Surface *surf, int px, int py, Uint32 color)
{
	if (SDL_MUSTLOCK(surf)) { SDL_LockSurface(surf); }
	if (px < 0 || py < 0 || px >= surf->w || py >= surf->h) return;
	int bytesperline = (surf->pitch / surf->format->BytesPerPixel);
	int index = py * bytesperline + px;
	Uint32 *ptra = (Uint32*)surf->pixels;
	ptra[index] = color;
	if (SDL_MUSTLOCK(surf)) { SDL_UnlockSurface(surf); }
	return;
}
*/
int getpixel(SDL_Surface *surf, int px, int py)
{
	if (SDL_MUSTLOCK(surf)) { SDL_LockSurface(surf); }
	int bytesperline = (surf->pitch / surf->format->BytesPerPixel);
	if (px < 0 || py < 0 ||	(px > (surf->w - 1) || py > (surf->h - 1))) return 0; // return 0xff000000;
	int index = py * bytesperline + px;
	Uint32 *ptra = (Uint32*)surf->pixels;
	Uint32 color = ptra[index];
	if (SDL_MUSTLOCK(surf)) { SDL_UnlockSurface(surf); }
	return color;
}

bool  isAlphaTransparent(int alfa)
{
	if (alfa < 128) return true;
	return false;
}
SDL_Surface* createNewSurfaceRGBA(int width, int height)
{
	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif
//	SDL_Surface *newsurface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0 );	// ERROR! no masking works...
	SDL_Surface *newsurface = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);	// ok.
	SDL_SetSurfaceBlendMode(newsurface, SDL_BLENDMODE_NONE);
	return newsurface;
}


// blink speed, timer value
//#define MY_BLINK_SPEED 15

//#include <cstdio>




// int alphautils_sinus_greyscale[32] = {
//	140, 159, 178, 195, 211, 223, 232, 238, 239, 237, 231, 222, 209, 193, 176,
//	157, 137, 117, 99, 81, 67, 55, 46, 41, 40, 42, 49, 59, 72, 88,
//	106, 125};

int alphautils_sinus_table[32] = {
	// sin
	140, 159, 178, 195, 211, 223, 232, 238, 239, 237, 231, 222, 209, 193, 176,
	157, 137, 117, 99, 81, 67, 55, 46, 41, 40, 42, 49, 59, 72, 88,
	106, 125
};
int alphautils_cosinus_table[32] = {
	//cos
	240, 238, 232, 222, 210, 194, 177, 158, 138, 119, 100, 82, 67, 55, 46,
	41, 40, 42, 48, 58, 71, 87, 105, 124, 143, 163, 182, 199, 213, 225,
	234, 238
};

unsigned int getTimedSinus()
{
	static int myindex = 0;
	static float scatch = 0;
	static float count = 9.0f;
	count = ImGui::GetIO().DeltaTime * 10.0f - scatch;
	count = 1.0f / count;
	static int oldcount = 0;
	static int delta = 0;
	int heltal = (int)count;
	int	sdelta = heltal - oldcount;
	if (sdelta >= 1 || sdelta <= -1) {myindex++;}
	oldcount = heltal;
	return alphautils_sinus_table[myindex & 31];	
}
unsigned int getTimedSinus180()
{
	static int myindex = 0;
	static float scatch = 0;
	static float count = 9.0f;
	count = ImGui::GetIO().DeltaTime * 10.0f - scatch;
	count = 1.0f / count;
	static int oldcount = 0;
	static int delta = 0;
	int heltal = (int)count;
	int	sdelta = heltal - oldcount;
	if (sdelta >= 1 || sdelta <= -1) {myindex++;	}
	oldcount = heltal;
	return alphautils_sinus_table[(myindex +16) & 31];
}

unsigned int getTimedSinus90()
{
	static int myindex = 0;
	static float scatch = 0;
	static float count = 9.0f;
	count = ImGui::GetIO().DeltaTime * 10.0f - scatch;
	count = 1.0f / count;
	static int oldcount = 0;
	static int delta = 0;
	int heltal = (int)count;
	int	sdelta = heltal - oldcount;
	if (sdelta >= 1 || sdelta <= -1) {myindex++;}
	oldcount = heltal;
	return alphautils_cosinus_table[myindex & 31];
}

unsigned int  getTimedBinary()
{
	static int myindex = 0;
	static float scatch = 0;
	static float count = 9.0f;
	count = ImGui::GetIO().DeltaTime * 10.0f - scatch;
	count = 1.0f / count;
	static int oldcount = 0;
	static int delta = 0;
	int heltal = (int)count;
	int	sdelta = heltal - oldcount;
	if (sdelta >= 1 || sdelta <= -1){ myindex++;}
	oldcount = heltal;
	myindex &= 31;
	if (myindex>15) return 255;
	return 0;
}







