#ifndef ZOOM_H
#define ZOOM_H

#include "imgui.h"

typedef struct Zoomstructure {
	float zoom;				// percentage of zoom
	ImVec2 sizexy;			// width x, height y
	ImVec2 fractionxy;		// 1.0 / width, 1.0/ height  (unused?)
	ImVec2 offsetxy;		// ox ( 0.0 to 1.0 ), oy ( 0.0 to 1.0 )
} Zoomstructure;



#endif