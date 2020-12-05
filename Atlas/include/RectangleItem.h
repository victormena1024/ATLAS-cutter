#ifndef RECTANGLEITEM_H
#define RECTANGLEITEM_H

#include "alphaUtils.h"
#include "imgui.h"
#include "zoom.h"
//#include <stdio.h> // for printf


class RectangleItem
{
public:
	RectangleItem();
	RectangleItem(ImVec2 begin, ImVec2 end, bool isSelected, bool _isHighlighted);
	~RectangleItem();
	void setZoomStructure(Zoomstructure *_zoomstr);
	bool isSelected();
	bool isHilighted();
	bool IsAboutToBeInverted();

	ImVec2 getBegin();
	ImVec2 getEnd();
	float getWidth();
	float getHeight();
	void  setWidth(float inw);
	void  setHeight(float inh);
	void setBegin(ImVec2 _begin);
	void setEnd(ImVec2 _end);
	
	void setBeginRaw(ImVec2 _begin);// without zoom
	void setEndRaw(ImVec2 _end);	// without zoom
	ImVec2 getBeginRaw();			// without zoom
	ImVec2 getEndRaw();				// without zoom
	float getWidthRaw();			// without zoom
	float getHeightRaw();			// without zoom
	void setWidthRaw(float inw);	// without zoom
	void setHeightRaw(float inh);	// without zoom

	int get_index_number();
	void set_index_number(int _index);

	void setSelected(bool _selected);
	void setHilighted(bool _hilighted);
	void setFlagIsAboutToBeInverted(bool);


	// calc is inside
	bool calcIsInside(ImVec2 _myPoint);		    // works ok
	int  calcWichInsideArea(ImVec2 _myPoint);	// 0 if outside, else 1 to 9
	void calc_knobs( ImVec2 offset, bool fromBufferToScreenCoords);
	void draw_knobs(ImDrawList* draw_list, int color);
	void draw_rectangle(ImDrawList* draw_list, ImVec2 canvas_pos, int colorset, int chosen_color, int chosen_color2, bool showFlashFlag);

	void add_coordinate_dxdy(float dx, float dy);
	void add_coordinate_dxdy(ImVec2 dxdy);

	// copy operator
	RectangleItem& operator=(const RectangleItem& other);

	/* translate coordinates*/
	ImVec2 from_buffer_to_screen_coords(ImVec2 source_buffer_pos);
	ImVec2 from_screen_coords_to_buffer(ImVec2 source_mouse_pos);
	ImVec2 scale_from_screen_coords_to_buffer(ImVec2 source_mouse_pos); // useful for dx,dy


private:
	bool isSelectedFlag = false;
	bool isHighlightedFlag = false;
	bool isInvertedFlag = false;
	ImVec2 begin;
	ImVec2 end;
	int  insideArea = 0;	// 0 is unused, else 1 to 9
	float ax1, ax2, ax3, ax4;
	float ay1, ay2, ay3, ay4;
	//
	Zoomstructure *zoomstr = nullptr;
	int	 indexnumber = 0;
	//

}; // class


#endif