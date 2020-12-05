// dear imgui: standalone example application for SDL2 + DirectX 11
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

//#define NEIVERR2
#ifdef  NEIVERR2


#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_syswm.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>




/*
// Demonstrate the various window flags. Typically you would just use the default!
	static bool no_titlebar = false;
	static bool no_scrollbar = false;
	static bool no_menu = false;
	static bool no_move = false;
	static bool no_resize = false;
	static bool no_collapse = false;
	static bool no_close = false;
	static bool no_nav = false;
	static bool no_background = false;
	static bool no_bring_to_front = false;

	if (ImGui::CollapsingHeader("Window options"))
	{
		ImGui::Checkbox("No titlebar", &no_titlebar); ImGui::SameLine(150);
		ImGui::Checkbox("No scrollbar", &no_scrollbar); ImGui::SameLine(300);
		ImGui::Checkbox("No menu", &no_menu);
		ImGui::Checkbox("No move", &no_move); ImGui::SameLine(150);
		ImGui::Checkbox("No resize", &no_resize); ImGui::SameLine(300);
		ImGui::Checkbox("No collapse", &no_collapse);
		ImGui::Checkbox("No close", &no_close); ImGui::SameLine(150);
		ImGui::Checkbox("No nav", &no_nav); ImGui::SameLine(300);
		ImGui::Checkbox("No background", &no_background);
		ImGui::Checkbox("No bring to front", &no_bring_to_front);
	}


	ImGuiWindowFlags window_flags = 0;
	if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
	if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
	if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
	if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
	if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
	if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
	if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	// Main body of the Demo window starts here.
	if (!ImGui::Begin("Dear ImGui Demo", p_open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}


*/






// Data
static ID3D11Device*            g_pd3dDevice = NULL;
static ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
static IDXGISwapChain*          g_pSwapChain = NULL;
static ID3D11RenderTargetView*  g_mainRenderTargetView = NULL;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

// Main code
int main(int, char**)
{
    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Setup window
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+DirectX11 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);
    HWND hwnd = (HWND)wmInfo.info.win.window;

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForD3D(window);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED && event.window.windowID == SDL_GetWindowID(window))
            {                
                // Release all outstanding references to the swap chain's buffers before resizing.
                CleanupRenderTarget();
                g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
                CreateRenderTarget();
            }
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}





// Simple helper function to load an image into a DX11 texture with common settings
bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);

	// STBIDEF stbi_uc *stbi_load (char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);

	if (image_data == NULL)
		return false;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D *pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	pTexture->Release();

	*out_width = image_width;
	*out_height = image_height;
	stbi_image_free(image_data);

	return true;
}


// unused function
void dx11implementation_mainloop() {		// gatito :o)

	static bool p_openx;

	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("DirectX11 Texture Test");

	ImVec2 imv = ImGui::GetCursorPos(); // drawing cursor on screen, used for overlay
	ImGui::Image((void*)my_texture, ImVec2((float)my_image_width, (float)my_image_height));
	ImGui::SetCursorPos(imv); // drawing cursor on screen, used for overlay

	// ...
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	// ...
	ImGui::Text("size = %d x %d", my_image_width, my_image_height);

	if (ImGui::IsMousePosValid())
		ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
	else
		ImGui::Text("Mouse Position: <invalid>");

	// Get the current cursor position (where your window is)
	ImVec2 position = ImGui::GetCursorScreenPos();
	ImGui::Text("crX = %d  crY = %d", (int)position.x, (int)position.y);

	ImVec2 w_pos = ImGui::GetCursorPos(), c_pos = ImGui::GetWindowPos();
	ImGui::Text("wX = %d  wY = %d", (int)w_pos.x, (int)w_pos.y);
	ImGui::Text("cX = %d  cY = %d", (int)c_pos.x, (int)c_pos.y);

	//	ImVec2 gradient_size = ImVec2(ImGui::CalcItemWidth(), ImGui::GetFrameHeight());
	//	ImGui::Text("grX = %d  grY = %d", (int)gradient_size.x, (int)gradient_size.y);

	/*
		ImGui::Button("80x80", ImVec2(80, 80));
		ImGui::SameLine();
		ImGui::Button("50x50", ImVec2(50, 50));
		ImGui::SameLine();
		ImGui::Button("Button()");
		ImGui::SameLine();
		ImGui::SmallButton("SmallButton()");
	*/

	/*
		// Manually wrapping
		// (we should eventually provide this as an automatic layout feature, but for now you can do it manually)
		ImVec2 button_sz(40, 40);
		ImGui::Text("Manually wrapping:");
		ImGuiStyle& style = ImGui::GetStyle();
		int buttons_count = 20;
		float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
		for (int n = 0; n < buttons_count; n++)
		{
			ImGui::PushID(n);
			ImGui::Button("Box", button_sz);
			float last_button_x2 = ImGui::GetItemRectMax().x;
			float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
			if (n + 1 < buttons_count && next_button_x2 < window_visible_x2)
				ImGui::SameLine();
			ImGui::PopID();
		}
	*/

	//	ImGui::Button("50x50", ImVec2(50, 50));
	//	ImGui::SameLine();

	float fx = io.MouseClickedPos->x;
	float fy = io.MouseClickedPos->y;

	ImGui::Text("[%f | %f]", fx, fy);
	//ImGui::Text("[%d | %d | %d | %d]", rect.top, rect.bottom, rect.left, rect.right);
	ImGui::Text("[%f | %f]", io.DisplaySize.x, io.DisplaySize.y);

	ImU32 color_red = ImColor(ImVec4(1.0f, 0.0f, 0.4f, 1.0f));
	ImU32 color_yellow = ImColor(ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	ImU32 color_blue = ImColor(ImVec4(0.0f, 0.0f, 1.0f, 1.0f));

	ImGui::GetOverlayDrawList()->AddCircleFilled(io.MousePos, 20, color_red);
	ImVec2 vmin = { 50,50 };
	ImVec2 vmax = { 200,200 };
	ImGui::GetOverlayDrawList()->AddRectFilled(vmin, vmax, color_yellow, 0, 0);

	/*
	Here’s one issue… GetWindowRect() is the wrong function to use it includes the size of window decoration
	(e.g. title screen). You want to use GetClientRect()
	*/

	/*
	So I guess the next best thing to try is to save the current draw cursor position by calling ImGui::GetCursorPos,
	then for each layer of the image reset the draw cursor position with ImGui::SetCursorPos before
	calling ImGui::Image for that layer.
	*/

	//	ImGuiWindow* window = GetCurrentWindow();
	//	if (window->SkipItems)	return false;
	//	ImGuiContext& g = *GImGui;
	//	const ImGuiID id = window->GetID(str_id);
	//	const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
	/*
		ImVec2 pos = ImGui::GetCursorScreenPos();
		float msx = ImGui::GetScrollX();
		float msy = ImGui::GetScrollY();
		ImGui::Text("msx = %f, msy = %f",msx,msy);
	*/
	/*
		int engine_screen_w = 1;
		int engine_screen_h = 1;

		// ********* //
	// tell the inbox if it should receive input
	//	bool info = ImGui::GetWindowIsFocused();
		/// move the draw cursor to the top left corner of the window
		ImGui::SetCursorPos(ImVec2(0, 0));

		// get window draw region
		ImVec2 win_region = ImGui::GetContentRegionAvail();
		/// send the region to engine inbox

		/// tell the engine the current draw position
	//	ImVec2 w_pos = ImGui::GetCursorPos(), c_pos = ImGui::GetWindowPos();

	//	EngineInbox.SetRenderingPosition(w_pos.x + c_pos.x, w_pos.y + c_pos.y);
	//	ImGui::Image((ImTextureID)GPU_GetImageHandle(engine_screen->image), ImVec2(engine_screen->w, engine_screen->h));
		/// tell the engine the current view portion
		ImVec2 uv1, uv2; /// top left and button down range ( 0 ... 1) like uv coords
		uv1.x = fabs(ImGui::GetWindowContentRegionMin().x) / engine_screen_w;
		uv1.y = fabs(ImGui::GetWindowContentRegionMin().y) / engine_screen_h;
		uv2.x = ImGui::GetContentRegionAvail().x / engine_screen_w + uv1.x;

		// ********* //
		ImGui::Text(" %d, %d, %d, %d", uv1.x, uv1.y, uv2.x, uv2.y );
		ImVec2 uik = ImGui::GetCursorStartPos();
	//	ImGui::SameLine();
		ImGui::Text(" %d, %d ", uik.x, uik.y);
		ImGui::Text(" %d, %d ", win_region.x, win_region.y);
	*/
	ImGui::End();
	// ImGuiButtonFlags_PressedOnClickRelease
}





/*
   /// tell the inbox if it should receive input
					EngineInbox.SetInputOn(ImGui::GetWindowIsFocused());
					/// move the draw cursor to the top left corner of the window
					ImGui::SetCursorPos(ImVec2(0,0));
					/// get window draw region
					ImVec2    win_region = ImGui::GetContentRegionAvail();
					/// send the region to engine inbox
					EngineInbox.SetRenderingWindowSize(win_region.x,win_region.y);
					/// tell the engine the current draw position
					ImVec2 w_pos = ImGui::GetCursorPos() ,c_pos = ImGui::GetWindowPos();
					EngineInbox.SetRenderingPosition(w_pos.x+c_pos.x,w_pos.y+c_pos.y);
					ImGui::Image((ImTextureID)GPU_GetImageHandle(engine_screen->image),ImVec2(engine_screen->w,engine_screen->h));
					/// tell the engine the current view potion
					ImVec2 uv1,uv2; /// top left and button down range ( 0 ... 1) like uv coords
					uv1.x = abs(ImGui::GetWindowContentRegionMin().x)/engine_screen->w;
					uv1.y = abs(ImGui::GetWindowContentRegionMin().y)/engine_screen->h;
					uv2.x = ImGui::GetContentRegionAvail().x / engine_screen->w +uv1.x;
					EngineInbox.SetViewPortion(uv1.x,uv1.y,uv2.x,uv2.y);
*/













#endif