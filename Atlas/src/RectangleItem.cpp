#include "RectangleItem.h"


RectangleItem::RectangleItem() {
	isSelectedFlag = false;
	isHighlightedFlag = false;
	isInvertedFlag = false;
	begin.x = begin.y = 0;
	end.x = end.y = 1;
	insideArea = 0;
	zoomstr = new Zoomstructure();
	zoomstr->zoom = 1.0f;
	zoomstr->sizexy.x = 100.0f;// my_image_width;
	zoomstr->sizexy.y = 100.0f;// my_image_height;
	zoomstr->fractionxy.x = zoomstr->sizexy.x / 100.0f;
	zoomstr->fractionxy.y = zoomstr->sizexy.y / 100.0f;
	zoomstr->offsetxy = { 0.0f,0.0f };
}

RectangleItem::RectangleItem(ImVec2 _begin, ImVec2 _end, bool _isSelected, bool _isHilighted) {
	isSelectedFlag = _isSelected;
	isHighlightedFlag = _isHilighted;
	begin = _begin;
	end = _end;
	insideArea = 0;
}

RectangleItem::~RectangleItem() {}

void RectangleItem::setZoomStructure(Zoomstructure *_zoomstr) {
	zoomstr = _zoomstr;
}

bool RectangleItem::isSelected() {
	return isSelectedFlag;
}
bool RectangleItem::isHilighted() {
	return isHighlightedFlag;
}



ImVec2 RectangleItem::getBeginRaw() {
	return begin;
}
ImVec2 RectangleItem::getEndRaw() {
	return end;
}
void RectangleItem::setBeginRaw(ImVec2 _begin) {
	begin = _begin;
}
void RectangleItem::setEndRaw(ImVec2 _end) {
	end = _end;
}

// DONE, apply zoomfactor into coordinates
ImVec2 RectangleItem::getBegin() { 
	ImVec2 retval;
	retval.x = (begin.x - zoomstr->offsetxy.x * zoomstr->sizexy.x) * zoomstr->zoom;
	retval.y = (begin.y - zoomstr->offsetxy.y * zoomstr->sizexy.y) * zoomstr->zoom;
	return retval;

}
ImVec2 RectangleItem::getEnd() { 
	ImVec2 retval;
	retval.x = (end.x - zoomstr->offsetxy.x * zoomstr->sizexy.x ) * zoomstr->zoom;
	retval.y = (end.y - zoomstr->offsetxy.y * zoomstr->sizexy.y ) * zoomstr->zoom;
	return retval;
}
void RectangleItem::setBegin(ImVec2 _begin) {
	begin.x = (_begin.x - zoomstr->offsetxy.x * zoomstr->sizexy.x ) * zoomstr->zoom;
	begin.y = (_begin.y - zoomstr->offsetxy.y * zoomstr->sizexy.y ) * zoomstr->zoom;
}
void RectangleItem::setEnd(ImVec2 _end){
	end.x = (_end.x - zoomstr->offsetxy.x * zoomstr->sizexy.x ) * zoomstr->zoom;
	end.y = (_end.y - zoomstr->offsetxy.y * zoomstr->sizexy.y ) * zoomstr->zoom;
}

float RectangleItem::getWidth() {
	return getEnd().x - getBegin().x;
}
float RectangleItem::getHeight() {
	return getEnd().y - getBegin().y;
}
void RectangleItem::setWidth(float inw) {
	float width = begin.x + inw;
	end.x = width;
}
void RectangleItem::setHeight(float inh) {
	float height = begin.y + inh;
	end.y = height;
}

float RectangleItem::getWidthRaw() {
	return end.x - begin.x;
}
float RectangleItem::getHeightRaw() {
	return end.y - begin.y;
}
void RectangleItem::setWidthRaw(float inw) {
	float width = begin.x + inw;
	end.x = width;
}
void RectangleItem::setHeightRaw(float inh) {
	float height = begin.y + inh;
	end.y = height;
}







void RectangleItem::setSelected(bool _selected){
	isSelectedFlag = _selected;
}
void RectangleItem::setHilighted(bool _hilighted) {
	isHighlightedFlag = _hilighted;
}
void RectangleItem::setFlagIsAboutToBeInverted(bool _isinv) {
	isInvertedFlag = _isinv;
}
bool RectangleItem::IsAboutToBeInverted(){
	return isInvertedFlag;
}



// returns true if _myPoint is inside rectangle
bool RectangleItem::calcIsInside(ImVec2 _myPoint) {
	float x1 = begin.x;
	float y1 = begin.y;
	float x2 = end.x;
	float y2 = end.y;
	float px = _myPoint.x;
	float py = _myPoint.y;
	if ( x1 < px &&	px < x2 ) {
		if (y1 < py &&	py < y2) {
			return true;
		}	
	}
	return false;
}


// works...
int  RectangleItem::calcWichInsideArea(ImVec2 _myPoint) {	// 0 if unselected else 1 to 9
	float px = _myPoint.x;
	float py = _myPoint.y;

// ********************************** //
	if (ax1 < px &&	px <= ax2) {
		if (ay1 < py &&	py <= ay2) {
			return 1;
		}
	}
	if (ax2 < px &&	px <= ax3) {
		if (ay1 < py &&	py <= ay2) {
			return 2;
		}
	}
	if (ax3 < px &&	px <= ax4) {
		if (ay1 < py &&	py <= ay2) {
			return 3;
		}
	}
// ********************************** //
	if (ax1 < px &&	px <= ax2) {
		if (ay2 < py &&	py <= ay3) {
			return 4;
		}
	}
	if (ax2 < px &&	px <= ax3) {
		if (ay2 < py &&	py <= ay3) {
			return 5;
		}
	}
	if (ax3 < px &&	px <= ax4) {
		if (ay2 < py &&	py <= ay3) {
			return 6;
		}
	}
// ********************************** //
	if (ax1 < px &&	px <= ax2) {
		if (ay3 < py &&	py <= ay4) {
			return 7;
		}
	}
	if (ax2 < px &&	px <= ax3) {
		if (ay3 < py &&	py <= ay4) {
			return 8;
		}
	}
	if (ax3 < px &&	px <= ax4) {
		if (ay3 < py &&	py <= ay4) {
			return 9;
		}
	}
// ********************************** //
	return 0;
}

/*
float RectangleItem::from_buffer_to_screen_coords_x(float inx)
{
	return (inx - zoomstr->offsetxy.x * zoomstr->sizexy.x) * zoomstr->zoom;
}
float RectangleItem::from_buffer_to_screen_coords_y(float iny)
{
	return (iny - zoomstr->offsetxy.y * zoomstr->sizexy.y) * zoomstr->zoom;
}
*/

ImVec2 RectangleItem::from_buffer_to_screen_coords(ImVec2 source_buffer_pos)
{
	float ix = (source_buffer_pos.x - zoomstr->offsetxy.x * zoomstr->sizexy.x) * zoomstr->zoom;
	float iy = (source_buffer_pos.y - zoomstr->offsetxy.y * zoomstr->sizexy.y) * zoomstr->zoom;
	return ImVec2{ix,iy};
}

// go from screen coordinates to buffer coordinates
ImVec2 RectangleItem::from_screen_coords_to_buffer(ImVec2 source_mouse_pos) {
	float ioks_x = zoomstr->sizexy.x * zoomstr->offsetxy.x;
	float ioks_y = zoomstr->sizexy.y * zoomstr->offsetxy.y;
	float retvalx = (source_mouse_pos.x + ioks_x * zoomstr->zoom) / zoomstr->zoom - ioks_x / zoomstr->zoom;
	float retvaly = (source_mouse_pos.y + ioks_y * zoomstr->zoom) / zoomstr->zoom - ioks_y / zoomstr->zoom;
	return ImVec2{ retvalx,retvaly };
}
ImVec2 RectangleItem::scale_from_screen_coords_to_buffer(ImVec2 source_mouse_pos) { // used by dx,dy code
	float ioks_x = 0.0f;
	float ioks_y = 0.0f;
	float retvalx = (source_mouse_pos.x + ioks_x * zoomstr->zoom) / zoomstr->zoom - ioks_x / zoomstr->zoom;
	float retvaly = (source_mouse_pos.y + ioks_y * zoomstr->zoom) / zoomstr->zoom - ioks_y / zoomstr->zoom;
	return ImVec2{ retvalx,retvaly };
}






// DONE, go from buffer coordinates to screen coordinates
void RectangleItem::calc_knobs( ImVec2 offset, bool fromBufferToScreenCoords )
{
	ax4 = end.x;
	ax1 = begin.x;
//	float fractionWidth = (end.x - begin.x) / 8.0f;
//	ax2 = ax1 + fractionWidth;
//	ax3 = ax4 - fractionWidth;
	ay4 = end.y;
	ay1 = begin.y;
//	float fractionHeight = (end.y - begin.y) / 8.0f;
//	ay2 = ay1 + fractionHeight;
//	ay3 = ay4 - fractionHeight;
	if (fromBufferToScreenCoords) {
		// DONE, go from buffer coordinates to screen coordinates
		ImVec2 retval;
		retval = from_buffer_to_screen_coords(ImVec2{ ax1,ay1 });
		ax1 = retval.x;
		ay1 = retval.y;
		retval = from_buffer_to_screen_coords(ImVec2{ ax4,ay4 });
		ax4 = retval.x;
		ay4 = retval.y;
		// DONE, go from buffer coordinates to screen coordinates
		ax1 += offset.x;
//		ax2 += offset.x;
//		ax3 += offset.x;
		ax4 += offset.x;
		ay1 += offset.y;
//		ay2 += offset.y;
//		ay3 += offset.y;
		ay4 += offset.y;
	}
}


//  0 = undefined (outside)
//  +-----+------+------+
//  |     |      |      |
//  |  1  |   2  |   3  |
//  |     |      |      |
//  +-----+------+------+
//  |     |      |      |
//  |  4  |   5  |   6  |
//  |     |      |      |
//  +-----+------+------+
//  |     |      |      |
//  |  7  |   8  |   9  |
//  |     |      |      |
//  +-----+------+------+
//
//

void drawSmallRect(ImDrawList* draw_list, ImVec2 coord, int color);

void drawSmallRect(ImDrawList* draw_list, ImVec2 coord, int color) {

	draw_list->AddRectFilled(ImVec2(coord.x - 4, coord.y - 4), ImVec2(coord.x + 4, coord.y + 4), color);
}


void RectangleItem::draw_knobs(ImDrawList* draw_list, int colour)
{
	int colourz[] =
	{
		(int)IM_COL32(150, 160, 180, 255),// grey = 0
		(int)IM_COL32(255, 0, 0, 255),	// red    = 1
		(int)IM_COL32(0, 0, 255, 255),	// blue   = 2
		(int)IM_COL32(0, 255, 0, 255)	// green  = 3
	};
	int color = colourz[colour & 3];
	// --- //
	drawSmallRect(draw_list, ImVec2(ax1, ay1), color);
//	drawSmallRect(draw_list, ImVec2(ax2, ay1), color);
//	drawSmallRect(draw_list, ImVec2(ax3, ay1), color);
	drawSmallRect(draw_list, ImVec2(ax4, ay1), color);
	// --- //
/*
	drawSmallRect(draw_list, ImVec2(ax1, ay2), color);
	drawSmallRect(draw_list, ImVec2(ax2, ay2), color);
	drawSmallRect(draw_list, ImVec2(ax3, ay2), color);
	drawSmallRect(draw_list, ImVec2(ax4, ay2), color);
	// --- //
	drawSmallRect(draw_list, ImVec2(ax1, ay3), color);
	drawSmallRect(draw_list, ImVec2(ax2, ay3), color);
	drawSmallRect(draw_list, ImVec2(ax3, ay3), color);
	drawSmallRect(draw_list, ImVec2(ax4, ay3), color);
*/
	// --- //
	drawSmallRect(draw_list, ImVec2(ax1, ay4), color);
//	drawSmallRect(draw_list, ImVec2(ax2, ay4), color);
//	drawSmallRect(draw_list, ImVec2(ax3, ay4), color);
	drawSmallRect(draw_list, ImVec2(ax4, ay4), color);
	// --- //
}


void RectangleItem::add_coordinate_dxdy(float dx, float dy)
{
	begin.x += dx;
	begin.y += dy;
	end.x += dx;
	end.y += dy;
}
void RectangleItem::add_coordinate_dxdy(ImVec2 dxdy)
{
	begin.x += dxdy.x;
	begin.y += dxdy.y;
	end.x += dxdy.x;
	end.y += dxdy.y;
}

// colorset is used by other function
void RectangleItem::draw_rectangle(ImDrawList* draw_list, ImVec2 canvas_pos, int colorset, int chosen_color1, int chosen_color2, bool showFlashFlag)
{
	float x1 = canvas_pos.x + getBegin().x;
	float y1 = canvas_pos.y + getBegin().y;
	float x2 = canvas_pos.x + getEnd().x;
	float y2 = canvas_pos.y + getEnd().y;
	if (isHilighted()) {					// this square is hilighted
		calc_knobs( canvas_pos, true );		// 
		draw_knobs( draw_list, colorset );	// colorset is either 2 or 1
	}

		if (isSelected()) {		// this square is selected
			float linewidth = 2.0f;
			float howmanydivisions = 11.0f;
			float width = x2 - x1;
			float height = y2 - y1;
			float rsx = width / howmanydivisions;
			float rsy = height / howmanydivisions;
			float x1a = x1;
			float x2a = x1 + rsx;
			float y1a = y1;
			float y2a = y1 + rsy;
			int i_howmany = (int) (howmanydivisions /  2.0f);
			for (int fi = 0;fi < i_howmany + 1;fi++) {
				draw_list->AddLine(ImVec2(x1a, y1), ImVec2(x2a, y1), chosen_color1, linewidth);
				draw_list->AddLine(ImVec2(x1a, y2), ImVec2(x2a, y2), chosen_color1, linewidth);
				if (fi >= i_howmany) {
					break;
				}
				draw_list->AddLine(ImVec2(x1a + rsx, y1), ImVec2(x2a + rsx, y1), chosen_color2, linewidth);
				draw_list->AddLine(ImVec2(x1a + rsx, y2), ImVec2(x2a + rsx, y2), chosen_color2, linewidth);
				x1a += rsx * 2;
				x2a += rsx * 2;
			}
			for (int fi = 0;fi < i_howmany + 1;fi++) {
				draw_list->AddLine(ImVec2(x1, y1a), ImVec2(x1, y2a), chosen_color1, linewidth);
				draw_list->AddLine(ImVec2(x2, y1a), ImVec2(x2, y2a), chosen_color1, linewidth);
				if (fi >= i_howmany) {
					break;
				}
				draw_list->AddLine(ImVec2(x1, y1a + rsy), ImVec2(x1, y2a + rsy), chosen_color2, linewidth);
				draw_list->AddLine(ImVec2(x2, y1a + rsy), ImVec2(x2, y2a + rsy), chosen_color2, linewidth);
				y1a += rsy * 2;
				y2a += rsy * 2;
			}
		}
	//}


//	if ( !showFlashFlag && isSelected()) {		// this square is selected
//		draw_list->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), chosen_color);		// greyscale fading
//	}

	if (!isSelected()) {		// this square is NOT selected (default), NORMAL SQUARE.
		draw_list->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), IM_COL32(255, 255, 0, 255)); // white
		draw_list->AddRect(ImVec2(x1 - 1.0f, y1 - 1.0f), ImVec2(x2 + 1.0f, y2 + 1.0f), IM_COL32(0, 255, 0, 255));		  // black
	}
}









RectangleItem& RectangleItem::operator=(const RectangleItem& other) // copy operator
{
	if (this != &other) {
//		printf("COPY OPERATOR Engaged\n");
		isSelectedFlag = other.isSelectedFlag;
		isHighlightedFlag = other.isHighlightedFlag;
		isInvertedFlag = other.isInvertedFlag;
		begin = other.begin;
		end = other.end;
		insideArea = other.insideArea;
		ax1 = other.ax1;
		ax2 = other.ax2;
		ax3 = other.ax3;
		ax4 = other.ax4;
		ay1 = other.ay1;
		ay2 = other.ay2;
		ay3 = other.ay3;
		ay4 = other.ay4;
		zoomstr = other.zoomstr;
		indexnumber = other.indexnumber;
	}
	return *this;
}


int RectangleItem::get_index_number() {
	return indexnumber;
}
void RectangleItem::set_index_number(int _index){
	indexnumber = _index;
}





