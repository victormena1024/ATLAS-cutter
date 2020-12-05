// dear imgui: standalone example application for SDL2 + DirectX 11
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_syswm.h>



#include "dx11implementation.h"



/*
		// Always center this window when appearing
//			ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
//			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowBgAlpha(1.0f);   	// set next window background color alpha.
			ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::AlignTextToFramePadding();
        ImGui::Text("New Name:");
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        bool apply = ImGui::InputText("##EditorContentBrowser", buf2, 64, ImGuiInputTextFlags_EnterReturnsTrue, NULL, NULL);
        ImGui::PopItemWidth();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))

*/




/*
ImGui::Begin("GameWindow");
{
  // Using a Child allow to fill all the space of the window.
  // It also alows customization
  ImGui::BeginChild("GameRender");
  // Get the size of the child (i.e. the whole draw size of the windows).
  ImVec2 wsize = ImGui::GetWindowSize();
  // Because I use the texture from OpenGL, I need to invert the V from the UV.
  ImGui::Image((ImTextureID)tex, wsize, ImVec2(0, 1), ImVec2(1, 0));
  ImGui::EndChild();
}
ImGui::End();
*/


void zShowExampleAppSimpleOverlay(bool* p_open);

/*
	int callback(ImGuiTextEditCallbackData* data)
{
    data->EventChar = 'A';
    return 0;
}
// ***********
	ImGui::InputText("Text", textArr.data(), textArr.size(), ImGuiInputTextFlags_CallbackCharFilter, callback);
// ++++++++++
	ImGui::InputText("Text", textArr.data(), textArr.size(),
	ImGuiInputTextFlags_CallbackCharFilter,
	[](ImGuiTextEditCallbackData* data)
	{
		data->EventChar = 'A';
		return 0;
	}
	);
*/



























//-----------------------------------------------------------------------------
// [SECTION] Example App: Simple Overlay / ShowExampleAppSimpleOverlay()
//-----------------------------------------------------------------------------

// Demonstrate creating a simple static window with no decoration
// + a context-menu to choose which corner of the screen to use.
static void zShowExampleAppSimpleOverlay(bool* p_open)
{
	return;

	const float DISTANCE = 10.0f;
	static int corner = 0;
	ImGuiIO& io = ImGui::GetIO();
	if (corner != -1)
	{
		ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
		ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	}
	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	if (corner != -1)
		window_flags |= ImGuiWindowFlags_NoMove;
	if (ImGui::Begin("Example: Simple overlay", p_open, window_flags))
	{
		ImGui::Text("Simple overlay\n" "in the corner of the screen.\n" "(right-click to change position)");
		ImGui::Separator();
		// io.BackendPlatformName;
		if (ImGui::IsMousePosValid())
			ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
		else
			ImGui::Text("Mouse Position: <invalid>");
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
			if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
			if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
			if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
			if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
			if (p_open && ImGui::MenuItem("Close")) *p_open = false;
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}


//-----------------------------------------------------------------------------
// [SECTION] Example App: Custom Rendering using ImDrawList API / ShowExampleAppCustomRendering()
//-----------------------------------------------------------------------------
// Demonstrate using the low-level ImDrawList to draw custom shapes.
void zzShowExampleAppCustomRendering(bool* p_open)
{
	ImGui::SetNextWindowSize(ImVec2(350, 560), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Example: Custom rendering", p_open))
	{
		ImGui::End();
		return;
	}

	// Tip: If you do a lot of custom rendering, you probably want to use your own geometrical types and benefit of overloaded operators, etc.
	// Define IM_VEC2_CLASS_EXTRA in imconfig.h to create implicit conversions between your types and ImVec2/ImVec4.
	// ImGui defines overloaded operators but they are internal to imgui.cpp and not exposed outside (to avoid messing with your types)
	// In this example we are not using the maths operators!
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	// Primitives
	ImGui::Text("Primitives");
	static float sz = 36.0f;
	static float thickness = 4.0f;
	static ImVec4 col = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
	ImGui::DragFloat("Size", &sz, 0.2f, 2.0f, 72.0f, "%.0f");
	ImGui::DragFloat("Thickness", &thickness, 0.05f, 1.0f, 8.0f, "%.02f");
	ImGui::ColorEdit3("Color", &col.x);
	{
		const ImVec2 p = ImGui::GetCursorScreenPos();
		const ImU32 col32 = ImColor(col);
		float x = p.x + 4.0f, y = p.y + 4.0f, spacing = 8.0f;
		for (int n = 0; n < 2; n++)
		{
			float curr_thickness = (n == 0) ? 1.0f : thickness;
			draw_list->AddCircle(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz*0.5f, col32, 20, curr_thickness); x += sz + spacing;
			draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, 0.0f, ImDrawCornerFlags_All, curr_thickness); x += sz + spacing;
			draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, 10.0f, ImDrawCornerFlags_All, curr_thickness); x += sz + spacing;
			draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, 10.0f, ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotRight, curr_thickness); x += sz + spacing;
			draw_list->AddTriangle(ImVec2(x + sz * 0.5f, y), ImVec2(x + sz, y + sz - 0.5f), ImVec2(x, y + sz - 0.5f), col32, curr_thickness); x += sz + spacing;
			draw_list->AddLine(ImVec2(x, y), ImVec2(x + sz, y), col32, curr_thickness); x += sz + spacing;   // Horizontal line (note: drawing a filled rectangle will be faster!)
			draw_list->AddLine(ImVec2(x, y), ImVec2(x, y + sz), col32, curr_thickness); x += spacing;      // Vertical line (note: drawing a filled rectangle will be faster!)
			draw_list->AddLine(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, curr_thickness); x += sz + spacing;   // Diagonal line
			draw_list->AddBezierCurve(ImVec2(x, y), ImVec2(x + sz * 1.3f, y + sz * 0.3f), ImVec2(x + sz - sz * 1.3f, y + sz - sz * 0.3f), ImVec2(x + sz, y + sz), col32, curr_thickness);
			x = p.x + 4;
			y += sz + spacing;
		}
		draw_list->AddCircleFilled(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz*0.5f, col32, 32); x += sz + spacing;
		draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col32); x += sz + spacing;
		draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, 10.0f); x += sz + spacing;
		draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, 10.0f, ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotRight); x += sz + spacing;
		draw_list->AddTriangleFilled(ImVec2(x + sz * 0.5f, y), ImVec2(x + sz, y + sz - 0.5f), ImVec2(x, y + sz - 0.5f), col32); x += sz + spacing;
		draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + thickness), col32); x += sz + spacing;          // Horizontal line (faster than AddLine, but only handle integer thickness)
		draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + thickness, y + sz), col32); x += spacing + spacing;     // Vertical line (faster than AddLine, but only handle integer thickness)
		draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + 1, y + 1), col32);          x += sz;                  // Pixel (faster than AddLine)
		draw_list->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + sz, y + sz), IM_COL32(0, 0, 0, 255), IM_COL32(255, 0, 0, 255), IM_COL32(255, 255, 0, 255), IM_COL32(0, 255, 0, 255));
		ImGui::Dummy(ImVec2((sz + spacing) * 8, (sz + spacing) * 3));
	}
	ImGui::Separator();
	{
		static ImVector<ImVec2> points;
		static bool adding_line = false;
		ImGui::Text("Canvas example");
		if (ImGui::Button("Clear")) points.clear();
		if (points.Size >= 2) { ImGui::SameLine(); if (ImGui::Button("Undo")) { points.pop_back(); points.pop_back(); } }
		ImGui::Text("Left-click and drag to add lines,\nRight-click to undo");

		// Here we are using InvisibleButton() as a convenience to 1) advance the cursor and 2) allows us to use IsItemHovered()
		// But you can also draw directly and poll mouse/keyboard by yourself. You can manipulate the cursor using GetCursorPos() and SetCursorPos().
		// If you only use the ImDrawList API, you can notify the owner window of its extends by using SetCursorPos(max).
		ImVec2 canvas_pos = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
		ImVec2 canvas_size = ImGui::GetContentRegionAvail();        // Resize canvas to what's available
		if (canvas_size.x < 50.0f) canvas_size.x = 50.0f;
		if (canvas_size.y < 50.0f) canvas_size.y = 50.0f;

		draw_list->AddRectFilledMultiColor(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(50, 50, 50, 255), IM_COL32(50, 50, 60, 255), IM_COL32(60, 60, 70, 255), IM_COL32(50, 50, 60, 255));
		draw_list->AddRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(255, 255, 255, 255));

		bool adding_preview = false;
		ImGui::InvisibleButton("canvas", canvas_size);
		ImVec2 mouse_pos_in_canvas = ImVec2(ImGui::GetIO().MousePos.x - canvas_pos.x, ImGui::GetIO().MousePos.y - canvas_pos.y);
		if (adding_line)
		{
			adding_preview = true;
			points.push_back(mouse_pos_in_canvas);
			if (!ImGui::IsMouseDown(0))
				adding_line = adding_preview = false;
		}
		if (ImGui::IsItemHovered())
		{
			if (!adding_line && ImGui::IsMouseClicked(0))
			{
				points.push_back(mouse_pos_in_canvas);
				adding_line = true;
			}
			if (ImGui::IsMouseClicked(1) && !points.empty())
			{
				adding_line = adding_preview = false;
				points.pop_back();
				points.pop_back();
			}
		}

		draw_list->PushClipRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), true);      // clip lines within the canvas (if we resize it, etc.)

		//draw_list->AddImage

//		draw_list->AddImage((void*)my_texture, ImVec2(my_image_width, my_image_height),ImVec2(my_image_width, my_image_height));

		for (int i = 0; i < points.Size - 1; i += 2) {
//			draw_list->AddLine(ImVec2(canvas_pos.x + points[i].x, canvas_pos.y + points[i].y), ImVec2(canvas_pos.x + points[i + 1].x, canvas_pos.y + points[i + 1].y), IM_COL32(255, 255, 0, 255), 2.0f);
			draw_list->AddRect(ImVec2(canvas_pos.x + points[i].x, canvas_pos.y + points[i].y), ImVec2(canvas_pos.x + points[i + 1].x, canvas_pos.y + points[i + 1].y), IM_COL32(75, 160, 200, 255));
		}
//		ImVec4 my_color = ImVec4(0.45f, 0.5f, 0.90f, 1.00f);
//		draw_list->AddRect(canvas_pos, canvas_size, my_color);

		draw_list->PopClipRect();
		if (adding_preview)
			points.pop_back();
	}
	ImGui::End();
}




// Demonstrate using the low-level ImDrawList to draw custom shapes.
static void zShowExampleAppCustomRendering(bool* p_open)
{
	if (!ImGui::Begin("Example: Custom rendering", p_open))
	{
		ImGui::End();
		return;
	}

	// Tip: If you do a lot of custom rendering, you probably want to use your own geometrical types and benefit of
	// overloaded operators, etc. Define IM_VEC2_CLASS_EXTRA in imconfig.h to create implicit conversions between your
	// types and ImVec2/ImVec4. Dear ImGui defines overloaded operators but they are internal to imgui.cpp and not
	// exposed outside (to avoid messing with your types) In this example we are not using the maths operators!
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	if (ImGui::BeginTabBar("##TabBar"))
	{
		if (ImGui::BeginTabItem("Primitives"))
		{
			ImGui::PushItemWidth(-ImGui::GetFontSize() * 10);

			// Draw gradients
			// (note that those are currently exacerbating our sRGB/Linear issues)
			ImGui::Text("Gradients");
			ImVec2 gradient_size = ImVec2(ImGui::CalcItemWidth(), ImGui::GetFrameHeight());
			{
				ImVec2 p0 = ImGui::GetCursorScreenPos();
				ImVec2 p1 = ImVec2(p0.x + gradient_size.x, p0.y + gradient_size.y);
				ImU32 col_a = ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
				ImU32 col_b = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
				draw_list->AddRectFilledMultiColor(p0, p1, col_a, col_b, col_b, col_a);
				ImGui::InvisibleButton("##gradient1", gradient_size);
			}
			{
				ImVec2 p0 = ImGui::GetCursorScreenPos();
				ImVec2 p1 = ImVec2(p0.x + gradient_size.x, p0.y + gradient_size.y);
				ImU32 col_a = ImGui::GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				ImU32 col_b = ImGui::GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				draw_list->AddRectFilledMultiColor(p0, p1, col_a, col_b, col_b, col_a);
				ImGui::InvisibleButton("##gradient2", gradient_size);
			}

			// Draw a bunch of primitives
			ImGui::Text("All primitives");
			static float sz = 36.0f;
			static float thickness = 3.0f;
			static int ngon_sides = 6;
			static bool circle_segments_override = false;
			static int circle_segments_override_v = 12;
			static ImVec4 colf = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
			ImGui::DragFloat("Size", &sz, 0.2f, 2.0f, 72.0f, "%.0f");
			ImGui::DragFloat("Thickness", &thickness, 0.05f, 1.0f, 8.0f, "%.02f");
			ImGui::SliderInt("N-gon sides", &ngon_sides, 3, 12);
			ImGui::Checkbox("##circlesegmentoverride", &circle_segments_override);
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
			if (ImGui::SliderInt("Circle segments", &circle_segments_override_v, 3, 40))
				circle_segments_override = true;
			ImGui::ColorEdit4("Color", &colf.x);
			const ImVec2 p = ImGui::GetCursorScreenPos();
			const ImU32 col = ImColor(colf);
			const float spacing = 10.0f;
			const ImDrawCornerFlags corners_none = 0;
			const ImDrawCornerFlags corners_all = ImDrawCornerFlags_All;
			const ImDrawCornerFlags corners_tl_br = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotRight;
			const int circle_segments = circle_segments_override ? circle_segments_override_v : 0;
			float x = p.x + 4.0f, y = p.y + 4.0f;
			for (int n = 0; n < 2; n++)
			{
				// First line uses a thickness of 1.0f, second line uses the configurable thickness
				float th = (n == 0) ? 1.0f : thickness;
				draw_list->AddNgon(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz*0.5f, col, ngon_sides, th);         x += sz + spacing;  // N-gon
				draw_list->AddCircle(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz*0.5f, col, circle_segments, th);  x += sz + spacing;  // Circle
				draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 0.0f, corners_none, th);     x += sz + spacing;  // Square
				draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 10.0f, corners_all, th);      x += sz + spacing;  // Square with all rounded corners
				draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 10.0f, corners_tl_br, th);    x += sz + spacing;  // Square with two rounded corners
				draw_list->AddTriangle(ImVec2(x + sz * 0.5f, y), ImVec2(x + sz, y + sz - 0.5f), ImVec2(x, y + sz - 0.5f), col, th);      x += sz + spacing;      // Triangle
				draw_list->AddTriangle(ImVec2(x + sz * 0.2f, y), ImVec2(x, y + sz - 0.5f), ImVec2(x + sz * 0.4f, y + sz - 0.5f), col, th); x += sz * 0.4f + spacing; // Thin triangle
				draw_list->AddLine(ImVec2(x, y), ImVec2(x + sz, y), col, th);                               x += sz + spacing;  // Horizontal line (note: drawing a filled rectangle will be faster!)
				draw_list->AddLine(ImVec2(x, y), ImVec2(x, y + sz), col, th);                               x += spacing;       // Vertical line (note: drawing a filled rectangle will be faster!)
				draw_list->AddLine(ImVec2(x, y), ImVec2(x + sz, y + sz), col, th);                          x += sz + spacing;  // Diagonal line
				draw_list->AddBezierCurve(ImVec2(x, y), ImVec2(x + sz * 1.3f, y + sz * 0.3f), ImVec2(x + sz - sz * 1.3f, y + sz - sz * 0.3f), ImVec2(x + sz, y + sz), col, th);
				x = p.x + 4;
				y += sz + spacing;
			}
			draw_list->AddNgonFilled(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz*0.5f, col, ngon_sides);   x += sz + spacing;  // N-gon
			draw_list->AddCircleFilled(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz*0.5f, col, circle_segments);x += sz + spacing;  // Circle
			draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col);                        x += sz + spacing;  // Square
			draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 10.0f);                 x += sz + spacing;  // Square with all rounded corners
			draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 10.0f, corners_tl_br);  x += sz + spacing;  // Square with two rounded corners
			draw_list->AddTriangleFilled(ImVec2(x + sz * 0.5f, y), ImVec2(x + sz, y + sz - 0.5f), ImVec2(x, y + sz - 0.5f), col);      x += sz + spacing;      // Triangle
			draw_list->AddTriangleFilled(ImVec2(x + sz * 0.2f, y), ImVec2(x, y + sz - 0.5f), ImVec2(x + sz * 0.4f, y + sz - 0.5f), col); x += sz * 0.4f + spacing; // Thin triangle
			draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + thickness), col);                 x += sz + spacing;  // Horizontal line (faster than AddLine, but only handle integer thickness)
			draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + thickness, y + sz), col);                 x += spacing * 2.0f;  // Vertical line (faster than AddLine, but only handle integer thickness)
			draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + 1, y + 1), col);                          x += sz;            // Pixel (faster than AddLine)
			draw_list->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + sz, y + sz), IM_COL32(0, 0, 0, 255), IM_COL32(255, 0, 0, 255), IM_COL32(255, 255, 0, 255), IM_COL32(0, 255, 0, 255));
			ImGui::Dummy(ImVec2((sz + spacing) * 9.8f, (sz + spacing) * 3));

			ImGui::PopItemWidth();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Canvas"))
		{
			static ImVector<ImVec2> points;
			static bool adding_line = false;
			if (ImGui::Button("Clear")) points.clear();
			if (points.Size >= 2) { ImGui::SameLine(); if (ImGui::Button("Undo")) { points.pop_back(); points.pop_back(); } }
			ImGui::Text("Left-click and drag to add lines,\nRight-click to undo");

			// Here we are using InvisibleButton() as a convenience to 1) advance the cursor and 2) allows us to use
			// IsItemHovered(). But you can also draw directly and poll mouse/keyboard by yourself.
			// You can manipulate the cursor using GetCursorPos() and SetCursorPos().
			// If you only use the ImDrawList API, you can notify the owner window of its extends with SetCursorPos(max).
			ImVec2 canvas_p = ImGui::GetCursorScreenPos();       // ImDrawList API uses screen coordinates!
			ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
			if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
			if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
			draw_list->AddRectFilledMultiColor(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y), IM_COL32(50, 50, 50, 255), IM_COL32(50, 50, 60, 255), IM_COL32(60, 60, 70, 255), IM_COL32(50, 50, 60, 255));
			draw_list->AddRect(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y), IM_COL32(255, 255, 255, 255));

			bool adding_preview = false;
			ImGui::InvisibleButton("canvas", canvas_sz);
			ImVec2 mouse_pos_global = ImGui::GetIO().MousePos;
			ImVec2 mouse_pos_canvas = ImVec2(mouse_pos_global.x - canvas_p.x, mouse_pos_global.y - canvas_p.y);
			if (adding_line)
			{
				adding_preview = true;
				points.push_back(mouse_pos_canvas);
				if (!ImGui::IsMouseDown(0))
					adding_line = adding_preview = false;
			}
			if (ImGui::IsItemHovered())
			{
				if (!adding_line && ImGui::IsMouseClicked(0))
				{
					points.push_back(mouse_pos_canvas);
					adding_line = true;
				}
				if (ImGui::IsMouseClicked(1) && !points.empty())
				{
					adding_line = adding_preview = false;
					points.pop_back();
					points.pop_back();
				}
			}

			// Draw all lines in the canvas (with a clipping rectangle so they don't stray out of it).
			draw_list->PushClipRect(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y), true);
			for (int i = 0; i < points.Size - 1; i += 2)
				draw_list->AddLine(ImVec2(canvas_p.x + points[i].x, canvas_p.y + points[i].y), ImVec2(canvas_p.x + points[i + 1].x, canvas_p.y + points[i + 1].y), IM_COL32(255, 255, 0, 255), 2.0f);
			draw_list->PopClipRect();
			if (adding_preview)
				points.pop_back();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("BG/FG draw lists"))
		{
			static bool draw_bg = true;
			static bool draw_fg = true;
			ImGui::Checkbox("Draw in Background draw list", &draw_bg);
			ImGui::SameLine(); 
//			HelpMarker("The Background draw list will be rendered below every Dear ImGui windows.");
			ImGui::Checkbox("Draw in Foreground draw list", &draw_fg);
			ImGui::SameLine(); 
//			HelpMarker("The Foreground draw list will be rendered over every Dear ImGui windows.");
			ImVec2 window_pos = ImGui::GetWindowPos();
			ImVec2 window_size = ImGui::GetWindowSize();
			ImVec2 window_center = ImVec2(window_pos.x + window_size.x * 0.5f, window_pos.y + window_size.y * 0.5f);
			if (draw_bg)
				ImGui::GetBackgroundDrawList()->AddCircle(window_center, window_size.x * 0.6f, IM_COL32(255, 0, 0, 200), 0, 10 + 4);
			if (draw_fg)
				ImGui::GetForegroundDrawList()->AddCircle(window_center, window_size.y * 0.6f, IM_COL32(0, 255, 0, 200), 0, 10);
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}






