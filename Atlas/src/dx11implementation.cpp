
// As a suggestion, you should use InvisibleButton() (or lower - level ButtonBehavior() to process inputs)

#include <iostream>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include "dx11implementation.h"

#include "RectangleItem.h"
#include "FileLoadAndSave.h"
#include "L2DFileDialog.h"
#include <vector>
#include "zoom.h"
#include <SDL_Image.h>

#include <filesystem>
#include "AlphaUtils.h"

//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>


bool isFileDialogActivated = false;		// its true when file select dialog is selected on top
//
static int selectedAlphaColor = 0;		// which color to use as alpha
//
Criteria criteria = Criteria::topleft;
//  Criteria::Topleft, means alpha value is whatever color is on top left pixel (0,0) in graphics
//	Criteria::Alphavalue, means use alpha value in 32 bit pixel values		 (not yet used)
//	Criteria::Magenta, means alpha value is full magenta, R=255, G=0, B=255, (not yet used)
//	Criteria::anycolor, means alpha value can be any color, R=?, G=?, B=?

static bool SelectAlphaFlag = false;		// select alpha color on one click
// DONE, select only one of these flags at a time.
// DONE, create "normal" mode flag ... yes
static bool NormalModeFlag = true;			// true on creating squares
static bool ToggleSelectionFlag = false;	// true when selecting squares, single or many
static bool MoveSelectedModeFlag = false;	// move selected squares
static bool ModifyModeFlag = false;			// hilight and modify on single click.
//
static bool showFilenameFlag = false;	// show filename in squares
static bool showNumericalFlag = false;	// show serial number in squares
static bool showPositionFlag = false;	// 
static bool showSizeFlag = false;		// 
static bool showFlashFlag = false;		// 

										
										//
static int highlightAreaNumber = 0;
static int highlightIndexNumber = 0;
static int lastSelectedRectangleIndex = -1;
//
// defaults on begin
static int px1 = 50;
static int py1 = 50;
static int glob_width = 50;
static int glob_height = 50;
static int spaceX = 20;
static int spaceY = 20;
static int numberOfX = 5;
static int numberOfY = 5;
//
static int	ppx = 0;
static int	ppy = 0;
static int  ppwidth = 0;
static int  ppheight = 0;
//
Zoomstructure *zoomstr = new Zoomstructure();
FileLoadAndSave *flaz = new FileLoadAndSave();

static char prefilename_str0[256] = "z";
static char postfilename_str1[256] = "png";
//
static int my_image_width = 0;
static int my_image_height = 0;
static ID3D11ShaderResourceView* my_texture = NULL;
static SDL_Surface *loadedAtlas = nullptr;

bool shutdown_and_restart = false;

static std::vector<RectangleItem *> ritemHolder;	// contains all squares
static std::vector<RectangleItem *> ritemHolder2;	// tmp storage
static std::vector<RectangleItem *> copypaste;		// copy selected and paste them into ritemholder
char		filebuffer[512] = { 0,0,0,0 };		//
static char sugarbuffer1[512] = " --- ";
static char sugarbuffer2[512] = " --- ";

// working extensions: ".bmp,.png,.jpg,.jpeg,.gif,.pcx,.tga,.lbm"
// prototypes
// DONE, expand Squares Vertical and Horizontal
void shrinkSquareVerticalRaw(RectangleItem *ritem);		// shrink
void shrinkSquareHorizontalRaw(RectangleItem *ritem);	// shrink
void expandSquareVerticalRaw(RectangleItem *ritem);		// expand
void expandSquareHorizontalRaw(RectangleItem *ritem);	// expand
void do_hplusplus();
void do_vplusplus();
void do_hminusminus();
void do_vminusminus();
void doActionZoomMinus();
void doActionZoomPlus();

void doShiftActionOnSelectedSquares(float addonX, float addonY);

ImVec2 from_buffer_to_screen_coords(ImVec2 source_buffer_pos);
ImVec2 from_screen_coords_to_buffer(ImVec2 source_mouse_pos);

void create_new_inifile_and_graphic_exit(char *filename, char *filepath);

std::string currentPath = ".";	// current path
std::string returnedPath = ".";	// returned current path
char *currentFile = nullptr;
//char *currentFile = (char *) "sprites.victor.scaled.bmp";
//const char *filename = "catinalps.tiff";	// dont work
//const char *filename = "catinalps.webp";	// dont work
//const char *filename = "catinalps.gif";	// works
//const char *filename = "catinalps.tga";	// works




void dx11implementation_init(char *filename_input) {
	// DONE, implement atlas filename loading and saving from configuration file
	flaz->set_ini_filename(filename_input);
	char   *auxiliary_filename_output = flaz->get_ini_base_filename();
	strncat(auxiliary_filename_output, ".bmp", 254);	// base + .bmp extension
	currentPath = std::filesystem::current_path().string();
	returnedPath = currentPath;

	bool bretval = flaz->open_ini_file_for_read();
	if (bretval) {
		flaz->readIniLineStrings();
		flaz->close_ini_file();
		char *fname = flaz->getIniLineString1();
		char *fpath = flaz->getIniLineString2();
		char *fline3 = flaz->getIniLineString3();	// width
		char *fline4 = flaz->getIniLineString4();	// height

	//	printf("filename: '%s'\n", fname);	
	//	printf("filepath: '%s'\n", fpath);	

		if (strlen(fname) == 0) {	// should not happen..., but it can happen anyway...
			printf("readed filename from ini file: '%s' is zero in length!. Rewriting .ini file with defaults.\n", flaz->get_ini_filename());
			create_new_inifile_and_graphic_exit(auxiliary_filename_output, (char *)currentPath.c_str());
		}
		else {
			// DONE, extract file path and copy it into currentpath string
			currentFile = fname; // 
			if (strlen(fpath) > 0) {
				currentPath = fpath;	//	ASSIGN
				returnedPath = currentPath;
			}
		}
	}
	else {	// input file don't exists, create new .ini file with default filename.
		create_new_inifile_and_graphic_exit(auxiliary_filename_output, (char *)currentPath.c_str());
	}


	// ...
	FILE *tst = fopen(currentFile, "r");
	if (tst == nullptr) {	// after reading .ini file, graphic file has been removed.
		printf("cant open file: '%s' ---> exit;\n", currentFile);
		// DONE, create new atlas.ini file and change the default filename
		create_new_inifile_and_graphic_exit(auxiliary_filename_output, (char *)currentPath.c_str());
	}
	else {
		fclose(tst);
	}


	bool ret = LoadTextureFromFile(currentFile, &my_texture, &my_image_width, &my_image_height);
	IM_ASSERT(ret);
	zoomstr->zoom = 1.0f;
	zoomstr->sizexy.x = (float)my_image_width;
	zoomstr->sizexy.y = (float)my_image_height;
	zoomstr->fractionxy.x = 1.0f / zoomstr->sizexy.x;
	zoomstr->fractionxy.y = 1.0f / zoomstr->sizexy.y;
	zoomstr->offsetxy = { 0.0f, 0.0f };
	SDL_Surface *tmp_atlas = IMG_Load(currentFile);
	if (tmp_atlas == nullptr) {
		printf("cant load Surface, using IMG_Load('%s'), delete '%s'...exit(-1)\n", currentFile, flaz->get_ini_filename());
		exit(-1);
	}
	printf("Loaded Texture: %s, bpp : %d\n", currentFile, tmp_atlas->format->BitsPerPixel);
	if (tmp_atlas->format->BitsPerPixel < 32) {
		criteria = Criteria::topleft;	// top left pixel value is used as alpha
	}
	else {		// 32 bits
		criteria = Criteria::alpha;		// use alpha from image
	}
	loadedAtlas = createNewSurfaceRGBA(tmp_atlas->w, tmp_atlas->h); // 32 bits
//	SDL_SetSurfaceBlendMode(loadedAtlas, SDL_BLENDMODE_NONE);
	SDL_SetSurfaceBlendMode(tmp_atlas, SDL_BLENDMODE_NONE);
	SDL_BlitSurface(tmp_atlas, NULL, loadedAtlas, NULL);		// go from 8,15,16 or 24 bits to 32 bits
	SDL_FreeSurface(tmp_atlas);
	selectedAlphaColor = getpixel(loadedAtlas, 0, 0);
}


void create_new_inifile_and_graphic_exit(char *filename, char *filepath) {
	if (strlen(filename) == 0) { printf("*ERROR empty filename* -> exit(-1)\n"); exit(-1); }
	//
	SDL_Surface *newsurface = SDL_CreateRGBSurface(0, 100, 100, 24, 0, 0, 0, 0);
	Uint32 backgroundcolor = SDL_MapRGBA(newsurface->format, 0xdd, 0xaa, 0x88, 0xff); // yellowish/red
	SDL_Rect square = { 0,0,100,100 };
	SDL_FillRect(newsurface, &square, backgroundcolor);
	backgroundcolor = SDL_MapRGBA(newsurface->format, 0x88, 0xdd, 0x99, 0xff);
	square = { 20,20,60,60 };
	SDL_FillRect(newsurface, &square, backgroundcolor);
	//
	SDL_SaveBMP(newsurface, filename);
	SDL_FreeSurface(newsurface);
	printf("created graphic file: '%s'\n", filename);
	printf("created new .ini file ('%s') with a filename of: '%s'\n", flaz->get_ini_filename(), filename);
	flaz->open_ini_file_for_write();
	flaz->writeIniLineString((char *)filename, (char *)filepath, (char *) "100", (char *) "100" );
	flaz->close_ini_file();
	printf("restart application to continue.\n");
	shutdown_and_restart = true;
	exit(0);
}





// return true on return key pressed
bool getInputIntegerValue(const char *textDisplay, const char *uniqueTextDisplay, int *bufferValue)
{
	if (*bufferValue < 0) *bufferValue = 0;
	char *buffer[17];
	int bufferSize = IM_ARRAYSIZE(buffer)-1;
	ImGui::Text(textDisplay);
	snprintf((char *)&buffer[0], bufferSize, "%d", *bufferValue);
	ImGui::SameLine();
	bool bretval = ImGui::InputText(uniqueTextDisplay, (char *)&buffer[0], bufferSize,
		ImGuiInputTextFlags_CharsDecimal |
		ImGuiInputTextFlags_EnterReturnsTrue);
	*bufferValue = atoi((char *)&buffer[0]);
	if (*bufferValue < 0) *bufferValue = 0;
	return bretval;
}

// return true on return key pressed
bool getInputFloatValue(const char *textDisplay, const char *uniqueTextDisplay, float *bufferValue)
{
	char *buffer[17];
	int bufferSize = IM_ARRAYSIZE(buffer)-1;
	ImGui::Text(textDisplay);
	snprintf((char *)&buffer[0], bufferSize, "%f", *bufferValue);
	ImGui::SameLine();
	bool bretval = ImGui::InputText(uniqueTextDisplay, (char *)&buffer[0], bufferSize,
		ImGuiInputTextFlags_CharsDecimal |
		ImGuiInputTextFlags_EnterReturnsTrue);
	*bufferValue = (float) atof((char *)&buffer[0]);
	return bretval;
}



char *createNewSerialNumber(char *prename, char *extension);
char *createNewSerialNumber2();
void setSerialNumber(int input);
void setSerialNumber2(int input);

static int serialnumber=0;
char *createNewSerialNumber( char *prename, char *extension )
{
	static char destination[256];
	snprintf((char*)&destination[0], IM_ARRAYSIZE(destination)-1, "%s%0.3d.%s", prename, serialnumber,extension  );
	serialnumber++;
	return (char *)&destination[0];
}
static int serialnumber2 = 0;
char *createNewSerialNumber2()
{
	static char destination2[26];
	snprintf((char*)&destination2[0], IM_ARRAYSIZE(destination2)-1, "%0.3d", serialnumber2 );
	serialnumber2++;
	return (char *)&destination2[0];
}

void setSerialNumber(int input) 
{
	serialnumber = input;
}
void setSerialNumber2(int input)
{
	serialnumber2 = input;
}







ImVec2 from_buffer_to_screen_coords(ImVec2 source_buffer_pos)
{
	float ix = (source_buffer_pos.x - zoomstr->offsetxy.x * zoomstr->sizexy.x) * zoomstr->zoom;
	float iy = (source_buffer_pos.y - zoomstr->offsetxy.y * zoomstr->sizexy.y) * zoomstr->zoom;
	return ImVec2{ ix,iy };
}
ImVec2 from_screen_coords_to_buffer(ImVec2 source_mouse_pos) {
	float ioks_x = zoomstr->sizexy.x * zoomstr->offsetxy.x;
	float ioks_y = zoomstr->sizexy.y * zoomstr->offsetxy.y;
	float retvalx = (source_mouse_pos.x + ioks_x * zoomstr->zoom) / zoomstr->zoom - ioks_x / zoomstr->zoom;
	float retvaly = (source_mouse_pos.y + ioks_y * zoomstr->zoom) / zoomstr->zoom - ioks_y / zoomstr->zoom;
	return ImVec2{retvalx,retvaly};
}
ImVec2 scale_from_screen_coords_to_buffer(ImVec2 source_mouse_pos) {
	float ioks_x = 0.0f;
	float ioks_y = 0.0f;
	float retvalx = (source_mouse_pos.x + ioks_x * zoomstr->zoom) / zoomstr->zoom - ioks_x / zoomstr->zoom;
	float retvaly = (source_mouse_pos.y + ioks_y * zoomstr->zoom) / zoomstr->zoom - ioks_y / zoomstr->zoom;
	return ImVec2{ retvalx,retvaly };
}
void invalidatelastSelectedRectangleIndex()
{
	lastSelectedRectangleIndex = -1;
	ppx = ppy = ppwidth = ppheight = 0;
}
// returns true if code should exit and restart
bool dx11_exitNow() {
	return shutdown_and_restart;
}




bool dx11_theButtonWindow()
{

	//	ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0 / 7.0f, 0.5f, 0.5f));
	//	ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0 / 7.0f, 0.5f, 0.5f));
	//	ImGui::PopStyleColor(1);
	//	static bool p_open = true;

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;


	//	window_flags |= ImGuiWindowFlags_NoBackground;
	ImGui::SetNextWindowBgAlpha(1.0f);   	// set next window background color alpha.
	ImGui::SetNextWindowPos(ImVec2(0, 20));

	ImGui::Begin("MainWindow", nullptr, window_flags);

	//	if (ImGui::IsItemActive() || ImGui::IsItemHovered()) ImGui::SetTooltip("%.3f", 7.6 );
	//	ImGui::InputText("##text1", buf1, bufSize);
	//  ImGui::Button("50x50", ImVec2(50, 50));
	//	static bool openfiledialog = true;
	//	static bool openpopup = false;

/*
//-----------------------------------------------------------------------------
// ImGuiStyle
// You may modify the ImGui::GetStyle() main instance during initialization and before NewFrame().
// During the frame, use ImGui::PushStyleVar(ImGuiStyleVar_XXXX)/PopStyleVar() to alter the main style values,
// and ImGui::PushStyleColor(ImGuiCol_XXX)/PopStyleColor() for colors.
//-----------------------------------------------------------------------------
	struct ImGuiStyle
	{
		float       Alpha;                      // Global alpha applies to everything in Dear ImGui.
		ImVec2      WindowPadding;              // Padding within a window.
		float       WindowRounding;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows. Large values tend to lead to variety of artifacts and are not recommended.
		float       WindowBorderSize;           // Thickness of border around windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
		ImVec2      WindowMinSize;              // Minimum window size. This is a global setting. If you want to constraint individual windows, use SetNextWindowSizeConstraints().
		// ~ FontSize + style.FramePadding.y * 2 + style.ItemSpacing.y (distance in pixels between 2 consecutive lines of framed widgets)
*/
//	sprintf(sugarbuffer1, "%d", style.FramePadding.x);

	if (ImGui::Button("LoadAtlas")) {
		ImGui::OpenPopup("OpenDialogPopup");
	}

	/*
	ImGuiWindowFlags_None                   = 0,
	ImGuiWindowFlags_NoTitleBar             = 1 << 0,   // Disable title-bar
	ImGuiWindowFlags_NoResize               = 1 << 1,   // Disable user resizing with the lower-right grip
	ImGuiWindowFlags_NoMove                 = 1 << 2,   // Disable user moving the window
	ImGuiWindowFlags_NoScrollbar            = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programmatically)
	ImGuiWindowFlags_NoScrollWithMouse      = 1 << 4,   // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
	ImGuiWindowFlags_NoCollapse             = 1 << 5,   // Disable user collapsing window by double-clicking on it
	ImGuiWindowFlags_AlwaysAutoResize       = 1 << 6,   // Resize every window to its content every frame
	ImGuiWindowFlags_NoBackground           = 1 << 7,   // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
	ImGuiWindowFlags_NoSavedSettings        = 1 << 8,   // Never load/save settings in .ini file
	ImGuiWindowFlags_NoMouseInputs          = 1 << 9,   // Disable catching mouse, hovering test with pass through.
	ImGuiWindowFlags_MenuBar                = 1 << 10,  // Has a menu-bar
	ImGuiWindowFlags_HorizontalScrollbar    = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
	ImGuiWindowFlags_NoFocusOnAppearing     = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
	ImGuiWindowFlags_NoBringToFrontOnFocus  = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
	ImGuiWindowFlags_AlwaysVerticalScrollbar= 1 << 14,  // Always show vertical scrollbar (even if ContentSize.y < Size.y)
	ImGuiWindowFlags_AlwaysHorizontalScrollbar=1<< 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
	ImGuiWindowFlags_AlwaysUseWindowPadding = 1 << 16,  // Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
	ImGuiWindowFlags_NoNavInputs            = 1 << 18,  // No gamepad/keyboard navigation within the window
	ImGuiWindowFlags_NoNavFocus             = 1 << 19,  // No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
	ImGuiWindowFlags_UnsavedDocument        = 1 << 20,  // Append '*' to title without affecting the ID, as a convenience to avoid using the ### operator. When used in a tab/docking context, tab is selected on closure and closure is deferred by one frame to allow code to cancel the closure (with a confirmation popup, etc.) without flicker.

	*/

	

	ImGui::SetNextWindowSize(ImVec2(740.0f, 410.0f));
	if (ImGui::BeginPopupModal("OpenDialogPopup", nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings
	)) {
		// current path is ok until here...		
		// DONE, scan for filename extensions and approve only selected extensions


		bool bretval = FileDialog::ShowFileDialog(filebuffer, IM_ARRAYSIZE(filebuffer) - 2, currentPath, returnedPath, isFileDialogActivated);	// returns empty filebuffer on "cancel"
		ImGui::EndPopup();
		if (bretval == true && strlen(filebuffer) > 0) {	// choose was selected, file name is valid
			// DONE, get graphic size from file and use it to store parameters
			int sx=100, sy=100;
			// todo, extract .defs from filename and check if string is not empty
			char *dot_exists = strrchr(filebuffer,'.');
			if (dot_exists) {				
				if (_strnicmp(dot_exists, ".defs", sizeof(filebuffer)-2) == 0) {
					dot_exists[0] = 0;	
				}
			}

			SDL_Surface *tmpsurface = IMG_Load( filebuffer );
			if (tmpsurface != nullptr) {
				sx = tmpsurface->w;
				sy = tmpsurface->h;
				SDL_FreeSurface(tmpsurface);
			}
			char bufferx[22];
			char buffery[22];
			snprintf(bufferx, sizeof(bufferx)-2,"%d", sx);
			snprintf(buffery, sizeof(buffery)-2,"%d", sy);
			//
			if (flaz->open_ini_file_for_write()) {
				flaz->writeIniLineString((char *)filebuffer, (char *)returnedPath.c_str(), bufferx, buffery );
				flaz->close_ini_file();
				printf("*w*.filebuffer    = '%s'\n", filebuffer);
				std::cout << "*w*.returned Path = '" << returnedPath << "'\n";
				shutdown_and_restart = true;	// DONE, restart application with new filename
			}
		}
		else {	// cancel was pressed, and standard return.
		}
	}





	ImGui::SameLine();
	if (ImGui::Button("save Selected rectangle files")) { // DONE: enable jpg functionality
		invalidatelastSelectedRectangleIndex();
		// # # # # # # # # # # # # # # # # # # # # # # # # # //
		float tmpfz = zoomstr->zoom;
		ImVec2 tmpoxoy = zoomstr->offsetxy;
		zoomstr->zoom = 1.0f;
		zoomstr->offsetxy = { 0.0f,0.0f };
		// # # # # # # # # # # # # # # # # # # # # # # # # # //
		setSerialNumber(0);
		if (ritemHolder.size() > 0) {
			printf("saved : filename,x,y,width,height\n");
		}
		for (int i = 0;i < (int)ritemHolder.size();i++) {
			RectangleItem *ritem = ritemHolder[i];
			ImVec2 begin = ritem->getBegin();
			ImVec2 end = ritem->getEnd();
			int width = (int)ritem->getWidth();
			int height = (int)ritem->getHeight();
			// DONE, use SDL blit and save squares
//			SDL_Surface *mySurface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, loadedAtlas->format->BitsPerPixel, 0, 0, 0, 0);
			SDL_Surface *mySurface = createNewSurfaceRGBA(width, height); // 32 bits
			SDL_Rect Rsrc;
			Rsrc.x = (int)begin.x;
			Rsrc.y = (int)begin.y;
			Rsrc.w = (int)width;
			Rsrc.h = (int)height;
			int retval = SDL_BlitSurface(loadedAtlas, &Rsrc, mySurface, nullptr);
			if (retval == 0 && ritem->isSelected()) {
				char *destination = createNewSerialNumber(prefilename_str0, postfilename_str1);
				char *extension = strrchr(postfilename_str1, '.'); // search for last dot character
				char destination2[512] = { 0,0 };
				strncpy(destination2, (char *)returnedPath.c_str(), 510);
				strncat(destination2, destination, 510);
				const char *comparable = nullptr;
				if (extension == nullptr) {	// not found, retain
					comparable = postfilename_str1;
				}
				else {					    // found dot character, copy
					comparable = extension;
				}
				bool notsaved = false;

				if (_strnicmp(comparable, "bmp", 254) == 0 || _strnicmp(comparable, ".bmp", 254) == 0) {
					SDL_SaveBMP(mySurface, destination2);
					notsaved = false;
				}
				else if (_strnicmp(comparable, "png", 254) == 0 || _strnicmp(comparable, ".png", 254) == 0) {
					IMG_SavePNG(mySurface, destination2);
					notsaved = false;
				}
				else if (_strnicmp(comparable, "jpg", 254) == 0 || _strnicmp(comparable, ".jpg", 254) == 0) {
					IMG_SaveJPG(mySurface, destination2, 100);	//quality = 100
					notsaved = false;
				}
				else {	// unrecognized extension
					notsaved = true;
				}
				if (notsaved) {
					std::cout << "*not saved, because extension is unrecognized, it is: " << comparable << "\n";
				}
				else {
					printf("%s,%d,%d,%d,%d\n", destination2, Rsrc.x, Rsrc.y, width, height);
				}
			}
		}
		// # # # # # # # # # # # # # # # # # # # # # # # # # //
		zoomstr->zoom = tmpfz;
		zoomstr->offsetxy = tmpoxoy;
		// # # # # # # # # # # # # # # # # # # # # # # # # # //
	}



// static int selectedAlphaColor = 0;		// which color to use as alpha

	ImGui::SameLine();
	if (ImGui::Button("save rectangle defs.")) {
		invalidatelastSelectedRectangleIndex();
		// # # # # # # # # # # # # # # # # # # # # # # # # # //
		float  tmpfz = zoomstr->zoom;
		ImVec2 tmpoxoy = zoomstr->offsetxy;
		float  tzoom = zoomstr->zoom;
		ImVec2 toff = zoomstr->offsetxy;
		zoomstr->zoom = 1.0f;
		zoomstr->offsetxy = { 0.0f, 0.0f };
		// # # # # # # # # # # # # # # # # # # # # # # # # # //
		FileLoadAndSave *flas = new FileLoadAndSave();
		flas->set_base_filename((char *)currentFile);
		printf("saving  '%s'\n", flas->get_base_filename());
		bool bretval = flas->open_rectangle_defs_for_write();
		if (bretval) {
			// ..............................
			flas->writeString((char *) "DEFS,"); //
			flas->writeString((char *)prefilename_str0); //
			flas->writeString((char *) ","); //
			flas->writeString((char *)postfilename_str1); //
			flas->writeString((char *) "\n"); //
	// ..............................
			flas->writeString((char *) "PARA,"); //
			flas->setParameters(px1, py1, glob_width, glob_height, spaceX, spaceY, numberOfX, numberOfY, tzoom, toff, selectedAlphaColor);
			flas->writeParametersIntoFilesystem();
			if (!ritemHolder.empty()) {
				for (int i = 0; i < (int)ritemHolder.size();i++) {
					RectangleItem *ritem = ritemHolder[i];
					ImVec2 begin = ritem->getBegin();
					ImVec2 end = ritem->getEnd();
					flas->writeString((char *) "RECT,"); //
					bool selected = ritem->isSelected();
					bool hilighted = ritem->isHilighted();
					hilighted = false;
					flas->setRectParameters(showFilenameFlag, showPositionFlag, showSizeFlag,
						selected, hilighted, begin, end);
					flas->writeRectParametersIntoFilesystem();
				}
			}
			flas->writeString((char *) "ENDE,\n"); //
			flas->close_rectangle_defs();
		}
		else {
			printf("*ERROR* saving '%s'\n", flas->get_base_filename());
		}
		// # # # # # # # # # # # # # # # # # # # # # # # # # //
		zoomstr->zoom = tmpfz;
		zoomstr->offsetxy = tmpoxoy;
		// # # # # # # # # # # # # # # # # # # # # # # # # # //
		delete flas;
	}



	ImGui::SameLine();
	if (ImGui::Button("load rectangle defs.")) {
		invalidatelastSelectedRectangleIndex();
		// # # # # # # # # # # # # # # # # # # # # # # # # # //
		float tmpfz = zoomstr->zoom;
		ImVec2 tmpoxoy = zoomstr->offsetxy;
		zoomstr->zoom = 1.0f;
		zoomstr->offsetxy = { 0.0f,0.0f };
		// # # # # # # # # # # # # # # # # # # # # # # # # # //
		float tzoom = 0.0f;
		ImVec2 toff;
		bool isZoomLoaded = false;
		ritemHolder.clear();
		FileLoadAndSave *flas = new FileLoadAndSave();
		flas->set_base_filename((char *)currentFile);
		bool bretval = flas->open_rectangle_defs_for_read();
		if (bretval) {
			printf("loading '%s'\n", flas->get_base_filename());
			flas->readLineString();		// read first string in file, must be DEFS,
			char *apHeader = flas->readStringHeader(); // get 4 bytes header
			if (strncmp(apHeader, "DEFS", 4) != 0) {
				printf("DEFS identity not found in file!...exit(1)\n");
				exit(1);
			}
			else {	// DEFS here
				char *prefilename = (char*)&prefilename_str0[0];
				char *postfilename = (char*)&postfilename_str1[0];
				flas->getDefsParameters(prefilename, postfilename);
				//			printf("pre: '%s',  post '%s'\n", prefilename, postfilename);
				strncpy(prefilename_str0, prefilename, 254);
				strncpy(postfilename_str1, postfilename, 254);
			}
			for (int ix = 0; ix < 999; ix++) {
				// flas->debugOn();
				char *myline = (char *)flas->readLineString(); // read into buffer
				int linelength = strlen(myline);
				if (linelength == 0) {
					flas->close_rectangle_defs();
					break;	// break out of loop, EOF reached
				}
				char *pHeader = flas->readStringHeader(); // get 4 bytes header
				if (strncmp(pHeader, "PARA", 4) == 0) {
					//			printf("...on PARA\n");
					flas->scanParametersFromBuffer();		// scan from buffer
					flas->getParameters(px1, py1, glob_width, glob_height, spaceX, spaceY, numberOfX, numberOfY, tzoom, toff, selectedAlphaColor);
				}
				if (strncmp(pHeader, "RECT", 4) == 0) {
					//				printf("...on RECT\n");
					// ...
					ImVec2 abegin;
					ImVec2 aend;
					bool aselected;
					bool ahilighted;
					flas->readRectParametersFromFilesystem();
					flas->getRectParameters(
						showFilenameFlag, showPositionFlag, showSizeFlag,
						aselected, ahilighted, abegin, aend);
					// ...
					RectangleItem *ritem = new RectangleItem(abegin, aend, aselected, ahilighted);
					ritem->setZoomStructure(zoomstr);
					ritem->setBegin(abegin);
					ritem->setEnd(aend);
					ritem->setSelected(aselected);
					// ritem->setHilighted(ahilighted);
					// ...
					ritemHolder.push_back(ritem);
				}
				if (strncmp(pHeader, "ENDE", 4) == 0) {
					//				printf("ENDE found, break\n");
					flas->close_rectangle_defs();
					//	update	zoomstr->zoom  and  zoomstr->offsetxy
//					zoomstr->zoom = tzoom;
//					zoomstr->offsetxy = toff;
					isZoomLoaded = true;
					break;	// end-of-file
				}
			}
		}
		else {
			printf("*ERROR* loading '%s'\n", flas->get_base_filename());
		}
		// # # # # # # # # # # # # # # # # # # # # # # # # # //
		if (isZoomLoaded) {
			zoomstr->zoom = tzoom;
			zoomstr->offsetxy = toff;
		}
		else {
			zoomstr->zoom = tmpfz;
			zoomstr->offsetxy = tmpoxoy;
		}

		// # # # # # # # # # # # # # # # # # # # # # # # # # //		
		delete flas;
	}


	//	ImGui::End();
	//	return true;


	ImGui::PushItemWidth(55.0f);
	//	ImGui::SetNextItemWidth(80.0f);


	ImGui::AlignTextToFramePadding();
	ImGui::Text("Filename short:");
	ImGui::SameLine();
	ImGui::InputText("##text1", prefilename_str0, IM_ARRAYSIZE(prefilename_str0) - 1, ImGuiInputTextFlags_CharsNoBlank);
	ImGui::SameLine();
	ImGui::Text("Extension:");
	ImGui::SameLine();
	ImGui::InputText("##text2", postfilename_str1, IM_ARRAYSIZE(postfilename_str1) - 1);

	/*
		int i = IM_ARRAYSIZE(prefilename_str0)-1;
		int j = IM_ARRAYSIZE(postfilename_str1)-1;
		snprintf(sugarbuffer1, 500, "IM_ARRAYSIZE(prefilename_str0) = %d", i);
		snprintf(sugarbuffer2, 500, "IM_ARRAYSIZE(prefilename_str1) = %d", j);
	*/

	ImGui::SameLine();
	char *destination = createNewSerialNumber(prefilename_str0, postfilename_str1);
	setSerialNumber(0);
	ImGui::Text("Total: ");
	ImGui::SameLine();
	ImGui::Text(destination);


	ImGui::AlignTextToFramePadding();
	bool bretval = getInputIntegerValue("X1", "###text3", &px1);
	ImGui::SameLine();
	bretval = getInputIntegerValue("Y1", "###text4", &py1);
	ImGui::SameLine();
	bretval = getInputIntegerValue("Width", "###text5", &glob_width);
	ImGui::SameLine();
	bretval = getInputIntegerValue("Height", "###text6", &glob_height);

	ImGui::AlignTextToFramePadding();
	bretval = getInputIntegerValue("spaceX", "###text7", &spaceX);
	ImGui::SameLine();
	bretval = getInputIntegerValue("spaceY", "###text8", &spaceY);
	ImGui::SameLine();
	bretval = getInputIntegerValue("Amount.X", "###text9", &numberOfX);
	ImGui::SameLine();
	bretval = getInputIntegerValue("Amount.Y", "###text10", &numberOfY);


	// ImGui::SameLine();
	if (ImGui::Button("Create New .H")) {
		invalidatelastSelectedRectangleIndex();
		int startx = px1;
		int starty = py1;
		for (int i = 0; i < numberOfX;i++) {
			int x1 = startx + (i * (glob_width + spaceX));
			int y1 = starty;
			int x2 = x1 + glob_width;
			int y2 = y1 + glob_height;
			RectangleItem *ritem = new RectangleItem(ImVec2((float)x1, (float)y1), ImVec2((float)x2, (float)y2), false, false);
			ritem->setZoomStructure(zoomstr);
			ritem->setSelected(true);
			ritemHolder.push_back(ritem);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Create New .V")) {
		invalidatelastSelectedRectangleIndex();
		int startx = px1;
		int starty = py1;
		for (int i = 0; i < numberOfY;i++) {
			int x1 = startx;
			int y1 = starty + (i * (glob_height + spaceY));
			int x2 = x1 + glob_width;
			int y2 = y1 + glob_height;
			RectangleItem *ritem = new RectangleItem(ImVec2((float)x1, (float)y1), ImVec2((float)x2, (float)y2), false, false);
			ritem->setZoomStructure(zoomstr);
			ritem->setSelected(true);
			ritemHolder.push_back(ritem);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Create New .H.V")) {
		invalidatelastSelectedRectangleIndex();

		int startx = px1;
		int starty = py1;
		for (int i = 0; i < numberOfY;i++) {
			for (int j = 0; j < numberOfX;j++) {
				int x1 = startx + (j * (glob_width + spaceX));
				int y1 = starty + (i * (glob_height + spaceY));
				int x2 = x1 + glob_width;
				int y2 = y1 + glob_height;
				RectangleItem *ritem = new RectangleItem(ImVec2((float)x1, (float)y1), ImVec2((float)x2, (float)y2), false, false);
				ritem->setZoomStructure(zoomstr);
				ritem->setSelected(true);
				ritemHolder.push_back(ritem);
			}
		}
	}

	// DONE, copy/paste of squares
	ImGui::SameLine();
	if (ImGui::Button("copy selected")) {
		invalidatelastSelectedRectangleIndex();
		copypaste.clear();	// empty storage	
		for (int i = 0;i < (int)ritemHolder.size(); i++) {
			RectangleItem *ritem = ritemHolder[i];
			RectangleItem *cap = nullptr;
			if (ritem->isSelected()) {
				ritem->setSelected(false);
				cap = new RectangleItem(ritem->getBeginRaw(), ritem->getEndRaw(), false, true);
				*cap = *ritem;	// copy data
				cap->add_coordinate_dxdy(12.0f, 12.0f);	// move +12 pixels in x and y
				copypaste.push_back(cap);
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("paste")) {
		invalidatelastSelectedRectangleIndex();
		for (int i = 0;i < (int)copypaste.size(); i++) {
			RectangleItem *cap = copypaste[i];
			cap->set_index_number(ritemHolder.size() + i);
			cap->setSelected(true);		// set selected 
			cap->setHilighted(false);	// no high light
			ritemHolder.push_back(cap);
		}
		copypaste.clear();
	}



//	ImGui::Checkbox("'Toggle Selection'", &ToggleSelectionFlag);
//	ImGui::SameLine();

	if (ImGui::Button("Invert All")) {
		invalidatelastSelectedRectangleIndex();
		for (int i = 0;i < (int)ritemHolder.size(); i++) {
			RectangleItem *ritem = ritemHolder[i];
			ritem->setSelected(!ritem->isSelected());
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Select All")) {
		invalidatelastSelectedRectangleIndex();
		for (int i = 0;i < (int)ritemHolder.size(); i++) {
			RectangleItem *ritem = ritemHolder[i];
			ritem->setSelected(true);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Select None")) {
		invalidatelastSelectedRectangleIndex();
		for (int i = 0;i < (int)ritemHolder.size(); i++) {
			RectangleItem *ritem = ritemHolder[i];
			ritem->setSelected(false);
		}
	}


	/*
		float mouseLeftHoldDownTime = 0.0f;
		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::IsMousePosValid()) {
			mouseLeftHoldDownTime = io.MouseDownDuration[0];	// button 0 (left), 1= right, 2=middle, 3=?,4=?,5=?
			for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) {	// how many mouse buttons, 5 currently
			}
		}
	*/

	//
	const float deltatimespeed = 10.0f;	// how fast we are counting
	const float itickertime = -0.8f;	// how much to wait until repeated action begins
	const float tickertime  =  0.5f;	// how much to wait between repeated actions
	//
	static int selectedButton = 0;		// 
	//
	static float tickerLeft  = itickertime;
	static float tickerRight = itickertime;
	static float tickerUp    = itickertime;
	static float tickerDown  = itickertime;
	static float tickerZoomM = itickertime;
	static float tickerZoomP = itickertime;
	static float tickerHplusplus = itickertime;
	static float tickerVplusplus = itickertime;
	static float tickerHminusminus = itickertime;
	static float tickerVminusminus = itickertime;

	//
	ImGui::SameLine();
	if (ImGui::Button("<<")) {
		doShiftActionOnSelectedSquares(-1.0f, 0.0f);
		tickerLeft = itickertime;
		selectedButton = 1;
	}
	if (ImGui::IsItemActive() && selectedButton==1 ) {
		tickerLeft += ImGui::GetIO().DeltaTime * deltatimespeed;
		if (tickerLeft > tickertime ) {	// tick
			tickerLeft = 0.0f;
			doShiftActionOnSelectedSquares(-1.0f, 0.0f);
		}
	}

	ImGui::SameLine();
	if (ImGui::Button(">>")) {
		doShiftActionOnSelectedSquares(1.0f, 0.0f);
		tickerRight = itickertime;
		selectedButton = 2;
	}
	if (ImGui::IsItemActive() && selectedButton == 2) {
		tickerRight += ImGui::GetIO().DeltaTime * deltatimespeed;
		if (tickerRight > tickertime) {	// tick
			tickerRight = 0.0f;
			doShiftActionOnSelectedSquares(1.0f, 0.0f);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("up")) {
		doShiftActionOnSelectedSquares(0.0f, -1.0f);
		tickerUp = itickertime;
		selectedButton = 3;
	}
	if (ImGui::IsItemActive() && selectedButton == 3) {
		tickerUp += ImGui::GetIO().DeltaTime * deltatimespeed;
		if (tickerUp > tickertime) {	// tick
			tickerUp = 0.0f;
			doShiftActionOnSelectedSquares(0.0f, -1.0f);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("dn")) {
		doShiftActionOnSelectedSquares(0.0f, +1.0f);
		tickerDown = itickertime;
		selectedButton = 4;
	}
	if (ImGui::IsItemActive() && selectedButton == 4) {
		tickerDown += ImGui::GetIO().DeltaTime * deltatimespeed;
		if (tickerDown > tickertime) {	// tick
			tickerDown = 0.0f;
			doShiftActionOnSelectedSquares(0.0f, +1.0f);
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("SelectAlpha")) {
		SelectAlphaFlag = true;
	}

	
	//
	// DONE, select only one of these flags at a time.
	if (ImGui::Checkbox("Normal Mode", &NormalModeFlag)) {
		if (NormalModeFlag == true) {
//			NormalModeFlag = false;
			ToggleSelectionFlag = false;
			MoveSelectedModeFlag = false;
			ModifyModeFlag = false;

			for (int i = 0;i < (int)ritemHolder.size(); i++) { ritemHolder[i]->setHilighted(false); }
			invalidatelastSelectedRectangleIndex();

		}
		else {
			NormalModeFlag = true;
		}
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Toggle Selection", &ToggleSelectionFlag)) {
		if (ToggleSelectionFlag == true) {
			NormalModeFlag = false;
//			ToggleSelectionFlag = false;
			MoveSelectedModeFlag = false;
			ModifyModeFlag = false;
		}
		else {
			NormalModeFlag = true;
		}
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Move Squares", &MoveSelectedModeFlag)) {
		if (MoveSelectedModeFlag == true) {
			NormalModeFlag = false;
			ToggleSelectionFlag = false;
//			MoveSelectedModeFlag = false;
			ModifyModeFlag = false;
		}
		else {
			NormalModeFlag = true;
		}
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Modify Mode", &ModifyModeFlag)) {
		if (ModifyModeFlag == true) {
			NormalModeFlag = false;
			ToggleSelectionFlag = false;
			MoveSelectedModeFlag = false;
//			ModifyModeFlag = false;
		}
		else {
			NormalModeFlag = true;
		}
	}

	//
	ImGui::Checkbox("Show Filename", &showFilenameFlag);
	ImGui::SameLine();
	ImGui::Checkbox("Show Numerical", &showNumericalFlag);
	ImGui::SameLine();
	ImGui::Checkbox("Show Position", &showPositionFlag);
	ImGui::SameLine();
	ImGui::Checkbox("Show Size",	 &showSizeFlag);


	if (ImGui::Button("Delete Selected")) {
		invalidatelastSelectedRectangleIndex();
		ritemHolder2.clear();	// tmp buffer
		// DONE, do delete
		// copy all non selected into tmp buffer
		for (int i = 0;i < (int)ritemHolder.size(); i++) {
			RectangleItem *ritem = ritemHolder[i];
			if (ritem->isSelected()) {		
			//	ritem->setSelected(false);
			}else {
				ritemHolder2.push_back(ritem);
			}
		}
		ritemHolder.clear();	// buffer
		// copy back from tmp buffer
		for (int i = 0;i < (int)ritemHolder2.size(); i++) {
			RectangleItem *ritem = ritemHolder2[i];
			ritemHolder.push_back(ritem);
		}
		ritemHolder2.clear();	// tmp buffer
	}



	ImGui::SameLine();
	if (ImGui::Button("Shrink H")) {
		for (int i = 0;i < (int)ritemHolder.size(); i++) {
			RectangleItem *ritem = ritemHolder[i];
			if (ritem->isSelected()) {
				shrinkSquareHorizontalRaw(ritem);
			}else {

			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Shrink V")) {
		for (int i = 0;i < (int)ritemHolder.size(); i++) {
			RectangleItem *ritem = ritemHolder[i];
			if (ritem->isSelected()) {
				shrinkSquareVerticalRaw(ritem); // , ritem->getBeginRaw(), ritem->getEndRaw());
			}else {
			
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Expand H")) {
		for (int i = 0;i < (int)ritemHolder.size(); i++) {
			RectangleItem *ritem = ritemHolder[i];
			if (ritem->isSelected()) {
				expandSquareHorizontalRaw(ritem);
			}
			else {

			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Expand V")) {
		for (int i = 0;i < (int)ritemHolder.size(); i++) {
			RectangleItem *ritem = ritemHolder[i];
			if (ritem->isSelected()) {
				expandSquareVerticalRaw(ritem); // , ritem->getBeginRaw(), ritem->getEndRaw());
			}
			else {

			}
		}
	}


	// static RectangleItem *ritem_passon = nullptr;

	ImGui::SameLine();
	if (ImGui::Button("H++")) {
		tickerHplusplus = itickertime;
		do_hplusplus();			// action	
	}
	if (ImGui::IsItemActive()) {
		tickerHplusplus += ImGui::GetIO().DeltaTime * deltatimespeed;
		if (tickerHplusplus > tickertime) {	// tick
			tickerHplusplus = 0.0f;
			do_hplusplus();			// action
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("V++")) {
		tickerVplusplus = itickertime;
		do_vplusplus();	// action	
	}
	if (ImGui::IsItemActive()) {
		tickerVplusplus += ImGui::GetIO().DeltaTime * deltatimespeed;
		if (tickerVplusplus > tickertime) {	// tick
			tickerVplusplus = 0.0f;
			do_vplusplus();	// action
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("H--")) {
		tickerHminusminus = itickertime;
		do_hminusminus();	// action	
	}
	if (ImGui::IsItemActive()) {
		tickerHminusminus += ImGui::GetIO().DeltaTime * deltatimespeed;
		if (tickerHminusminus > tickertime) {	// tick
			tickerHminusminus = 0.0f;
			do_hminusminus();	// action
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("V--")) {
		tickerVminusminus = itickertime;
		do_vminusminus();	// action	
	}
	if (ImGui::IsItemActive()) {
		tickerVminusminus += ImGui::GetIO().DeltaTime * deltatimespeed;
		if (tickerVminusminus > tickertime) {	// tick
			tickerVminusminus = 0.0f;
			do_vminusminus();	// action
		}
	}








	ImGui::SameLine();
	if (ImGui::Button("<re-sort>")) {
		ritemHolder2.clear();  // tmp buffer
		// copy all selected into tmp buffer
		for (int i = 0;i < (int)ritemHolder.size(); i++) {
			RectangleItem *ritem = ritemHolder[i];
			if (ritem->isSelected()) {
				ritem->set_index_number(i);
				ritemHolder2.push_back(ritem);
			}
		}
		RectangleItem *ritem3 = new RectangleItem();
		// bubble sort on temporary buffer
		for (int j = 0;j < (int)ritemHolder2.size(); j++) {
			for (int i = 0;i < (int)ritemHolder2.size() - 1; i++) {
				RectangleItem *ritem1 = ritemHolder2[i];			
				RectangleItem *ritem2 = ritemHolder2[i + 1];
				if (ritem1->getBeginRaw().x > ritem2->getBeginRaw().x) { // swap positions
					int index1 = ritem1->get_index_number();
					int index2 = ritem2->get_index_number();
					*ritem3 = *ritem2;	// use copy operator
					*ritem2 = *ritem1;	// use copy operator
					*ritem1 = *ritem3;	// use copy operator
					ritem1->set_index_number(index1);
					ritem2->set_index_number(index2);
				}
			}
		}
		delete ritem3;
		// copy back from tmp buffer
		for (int i = 0;i < (int)ritemHolder2.size(); i++) {
			RectangleItem *ritem2 = ritemHolder2[i];
			ritemHolder[ritem2->get_index_number()] = ritem2;
		}
		ritemHolder2.clear();	// tmp buffer
	}



	ImGui::AlignTextToFramePadding();
	bretval = getInputIntegerValue("X", "###text13", &ppx);
	ImGui::SameLine();
	bretval = getInputIntegerValue("Y", "###text14", &ppy);
	ImGui::SameLine();
	bretval = getInputIntegerValue("Width", "###text15", &ppwidth);
	ImGui::SameLine();
	bretval = getInputIntegerValue("Height", "###text16", &ppheight);
	ImGui::SameLine();
	if (ImGui::Button(">>> Modify")) {
		if (lastSelectedRectangleIndex != -1) {
//			RectangleItem * ritm = ritemHolder[ lastSelectedRectangleIndex ];
			RectangleItem * ritm = ritemHolder.at(lastSelectedRectangleIndex);
			ritm->setBeginRaw(ImVec2( (float) ppx, (float) ppy) );
			ritm->setWidthRaw(  (float) ppwidth  );
			ritm->setHeightRaw( (float) ppheight );
		}

	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Flash", &showFlashFlag)) {
	}



	ImGui::AlignTextToFramePadding();
	bretval = getInputFloatValue("zoom", "###zoom1", &zoomstr->zoom);
	ImGui::SameLine();
	if (ImGui::Button("Zoom -")) {
		doActionZoomMinus();
		tickerZoomM = itickertime;
	}
	// -----------------
	if (ImGui::IsItemActive()) {
		tickerZoomM += ImGui::GetIO().DeltaTime * deltatimespeed;
		if (tickerZoomM > tickertime) {	// tick
			tickerZoomM = 0.0f;
			doActionZoomMinus();
		}
	}
	// -----------------
	ImGui::SameLine();
	if (ImGui::Button("Zoom +")) {
		doActionZoomPlus();
		tickerZoomP = itickertime;
	}
	// -----------------
	if (ImGui::IsItemActive()) {
		tickerZoomP += ImGui::GetIO().DeltaTime * deltatimespeed;
		if (tickerZoomP > tickertime) {	// tick
			tickerZoomP = 0.0f;
			doActionZoomPlus();
		}
	}
	// -----------------

	   	  
	ImGui::PushItemWidth(640);
	ImGui::SliderFloat("X", &zoomstr->offsetxy.x, 0.0f, 1.0f); //	ImGui::SameLine(0,10);
	ImGui::SliderFloat("Y", &zoomstr->offsetxy.y, 0.0f, 1.0f);
	ImGui::PopItemWidth();

	ImGuiIO& io = ImGui::GetIO();
	if (isFileDialogActivated==false && io.MouseWheel != 0.0f) {
		zoomstr->offsetxy.y -= io.MouseWheel / 120.0f;
		if (zoomstr->offsetxy.y < 0)
			zoomstr->offsetxy.y = 0.0f;
		if (zoomstr->offsetxy.y > 1.0f)
			zoomstr->offsetxy.y = 1.0f;
	}

	return true;
}


void doShiftActionOnSelectedSquares( float addonX, float addonY) 
{
	for (int i = 0;i < (int)ritemHolder.size(); i++) {
		RectangleItem *ritem = ritemHolder[i];
		if (ritem->isSelected()) {
			ImVec2 begin = ritem->getBeginRaw();
			ImVec2 end = ritem->getEndRaw();
			begin.x += addonX;	end.x += addonX;
			begin.y += addonY;	end.y += addonY;
			ritem->setBeginRaw(begin);
			ritem->setEndRaw(end);
		}
	}
}




// shrink a square by reading the pixel bits and discriminating the alpha bits
void shrinkSquareVerticalRaw(RectangleItem *ritem) {
	float
		x1 = ritem->getBeginRaw().x,
		y1 = ritem->getBeginRaw().y,
		x2 = ritem->getEndRaw().x,
		y2 = ritem->getEndRaw().y;
	float width  = ritem->getWidthRaw();
	float height = ritem->getHeightRaw();
	float offsety = 0.0f;
	// ************************ //
	// ************************ //
	// ************************ //
	for (float y = 0;y < height / 2.0f;y+=1.0f) {
		bool isAlphaPixelFound = true;
		y1 = ritem->getBeginRaw().y;
		for (float x = 0.0f; x < width; x+=1.0f) {
			int alpha = getAlphaValue(loadedAtlas, getpixel(loadedAtlas, (int)(x1 + x), (int)y1), criteria, selectedAlphaColor);
			if (alpha < 127) {
				isAlphaPixelFound = true; // regular transparent
			}
			else {
				isAlphaPixelFound = false; // opaque
				break;
			}
		} // for x
		if (isAlphaPixelFound == true) {
			y1 += 1.0f;
			ritem->setBeginRaw( ImVec2{x1,y1} );
		}
		else {	// break out of for loop
			break;
		}
	} // for y
	// ************************ //
	// ************************ //
	// ************************ //
	for (float y = 0;y < height / 2.0f; y+=1.0f) {
		if (y == 1.0f) offsety = 1.0f;
		bool isAlphaPixelFound = true;
		y2 = ritem->getEndRaw().y;
		for (float x = 0.0f; x < width; x+=1.0f) {
			int alpha = getAlphaValue(loadedAtlas, getpixel(loadedAtlas, (int)(x1 + x), (int)y2), criteria, selectedAlphaColor);
			if (alpha < 127) {
				isAlphaPixelFound = true; // regular transparent
			}
			else {
				isAlphaPixelFound = false; // opaque
				break;
			}
		} // for x
		if (isAlphaPixelFound == true) {
			y2 -= 1.0f;
			ritem->setEndRaw( ImVec2{ x2,y2 });
		}
		else {	// break out of for loop
			break;
		}
	} // for y
	y2 = ritem->getEndRaw().y + offsety;
	ritem->setEndRaw(ImVec2(x2, y2));
}




// shrink a square by reading the pixel bits and discriminating the alpha bits
void shrinkSquareHorizontalRaw(RectangleItem *ritem) {
	float
		x1 = ritem->getBeginRaw().x,
		y1 = ritem->getBeginRaw().y,
		x2 = ritem->getEndRaw().x,
		y2 = ritem->getEndRaw().y;
	float width = ritem->getWidthRaw();
	float height = ritem->getHeightRaw();
	float offsetx = 0.0f;
	//
	// ************************ //
	// ************************ //
	// ************************ //
	for (float x = 0.0f;x < width / 2.0f;x+=1.0f) {
		bool isAlphaPixelFound = true;
		x1 = ritem->getBeginRaw().x;
		for (float y = 0; y < height; y++) {
			float yy = y1 + y;
			int alpha = getAlphaValue(loadedAtlas, getpixel(loadedAtlas, (int)x1, (int)yy), criteria, selectedAlphaColor);
			if (alpha < 127) {
				isAlphaPixelFound = true; // regular transparent
			}
			else {
				isAlphaPixelFound = false; // opaque
				
				break;
			}
		} // for y
		if (isAlphaPixelFound == true) {
			x1 += 1.0f;
			ritem->setBeginRaw( ImVec2{x1,y1}  );
		}
		else {	// break out of for loop
			break;
		}
	} // for x
	// ************************ //
	// ************************ //
	// ************************ //
	for (float x = 0.0f;x < width / 2.0f;x += 1.0f ) {
		if (x == 1.0f) offsetx = 1.0f;
		bool isAlphaPixelFound = true;
		x2 = ritem->getEndRaw().x;
		for (float y = 0; y < height; y++) {
			float yy = y1 + y;
			int alpha = getAlphaValue(loadedAtlas, getpixel(loadedAtlas, (int)x2, (int)yy), criteria, selectedAlphaColor);
			if (alpha < 127) {
				isAlphaPixelFound = true; // regular transparent
			}
			else {
				isAlphaPixelFound = false; // opaque
				break;
			}
		} // for y
		if (isAlphaPixelFound == true) {
			x2 -= 1.0f;
			ritem->setEndRaw(ImVec2{ x2,y2 });
//			x2 -= 1.0f;
		}
		else {	// break out of for loop
			break;
		}
	} // for x
	x2 = ritem->getEndRaw().x + offsetx;
	ritem->setEndRaw(ImVec2(x2, y2));
}































void doActionZoomMinus() {
	zoomstr->zoom -= 0.05f;
	if (zoomstr->zoom < 0.05f) zoomstr->zoom = 0.05f;
}
void doActionZoomPlus() {
	zoomstr->zoom += 0.05f;
	if (zoomstr->zoom > 8.0f) zoomstr->zoom = 8.0f;
}







void set_flag_IsAboutToBeInverted(ImVec2 anchor) {
	for (int i = 0; i < (int)ritemHolder.size(); i++) {
		RectangleItem *ritem = ritemHolder[i];
		if (ritem->calcIsInside(anchor)) {
			ritem->setFlagIsAboutToBeInverted(true);
			break;
		}
	}
}
void set_flag_IsAboutToBeInvertedInsideContainer(ImVec2 _b, ImVec2 _f){
	ImVec2 myBegin;
	ImVec2 myFinal;
	for (int i = 0; i < (int)ritemHolder.size(); i++) {
		RectangleItem *ritem = ritemHolder[i];
		myBegin = ritem->getBeginRaw();
		myFinal = ritem->getEndRaw();
		if (myBegin.x > _b.x &&
			myBegin.y > _b.y &&
			myFinal.x < _f.x &&
			myFinal.y < _f.y) {
			ritem->setFlagIsAboutToBeInverted(true);
		}
	}
}
void set_flag_IsAboutToBeInvertedInsideHalfSlice(ImVec2 _b, ImVec2 _f) {
	ImVec2 myBegin;
	ImVec2 myFinal;
	for (int i = 0; i < (int)ritemHolder.size(); i++) {
		RectangleItem *ritem = ritemHolder[i];
		myBegin = ritem->getBeginRaw();
		myFinal = ritem->getEndRaw();
		if (myBegin.x > _b.x &&	// horizontal. top
			myFinal.x < _f.x &&
			myBegin.y < _b.y &&
			myFinal.y > _b.y ){
			ritem->setFlagIsAboutToBeInverted(true);
		}
		if (myBegin.x > _b.x &&	// horizontal.bottom
			myFinal.x < _f.x &&
			myBegin.y < _f.y &&
			myFinal.y > _b.y ){
			ritem->setFlagIsAboutToBeInverted(true);
		}
		if (myBegin.y > _b.y &&	// vertical.left
			myFinal.y < _f.y &&
			myBegin.x < _b.x &&
			myFinal.x > _b.x ){
			ritem->setFlagIsAboutToBeInverted(true);
		}
		if (myBegin.x < _f.x &&	// vertical.right
			myFinal.x > _f.x &&
			myBegin.y > _b.y &&
			myFinal.y < _f.y ){
			ritem->setFlagIsAboutToBeInverted(true);
		}
	}
}



/*
ImVec2 scale_from_screen_coords_to_buffer(ImVec2 source_mouse_pos) {
	float ioks_x = 0.0f;
	float ioks_y = 0.0f;
	float retvalx = (source_mouse_pos.x + ioks_x * zoomstr->zoom) / zoomstr->zoom - ioks_x / zoomstr->zoom;
	float retvaly = (source_mouse_pos.y + ioks_y * zoomstr->zoom) / zoomstr->zoom - ioks_y / zoomstr->zoom;
	return ImVec2{ retvalx,retvaly };
}
*/




void dx11_theToolWindow()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiContext& g = *GImGui;

//	io.KeyRepeatDelay = 0;

	ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);

	// Tip: If you do a lot of custom rendering, you probably want to use your own geometrical types and benefit of overloaded operators, etc.
	// Define IM_VEC2_CLASS_EXTRA in imconfig.h to create implicit conversions between your types and ImVec2/ImVec4.
	// ImGui defines overloaded operators but they are internal to imgui.cpp and not exposed outside (to avoid messing with your types)
	// In this example we are not using the maths operators!

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	{
		//		ImVec2 imv = ImGui::GetCursorPos(); // drawing cursor on screen, used for overlay
		//		ImGui::Image((void*)my_texture, ImVec2(my_image_width, my_image_height));
		//		ImGui::SetCursorPos(imv); // drawing cursor on screen, used for overlay

		static bool mouseStatusCLICKOFF = false;
		static bool mouseStatusCLICKON = false;
		static bool mouseStatusDRAG = false;

		ImGui::Text(sugarbuffer1);
		ImGui::SameLine();
		ImGui::Text(sugarbuffer2);

		ImVec2 canvas_pos = ImGui::GetCursorScreenPos();           // ImDrawList API uses screen coordinates!
		ImVec2 canvas_size = ImGui::GetContentRegionAvail();        // Resize canvas to what's available
		if (canvas_size.x < 100.0f) canvas_size.x = 100.0f;
		if (canvas_size.y < 100.0f) canvas_size.y = 100.0f;
		float ioks_x = zoomstr->offsetxy.x * zoomstr->sizexy.x;
		float ioks_y = zoomstr->offsetxy.y * zoomstr->sizexy.y;
		float oks_x = zoomstr->zoom * (zoomstr->sizexy.x - ioks_x);
		float oks_x_end = zoomstr->zoom * (zoomstr->sizexy.x);
		float oks_y = zoomstr->zoom * (zoomstr->sizexy.y - ioks_y);
		float oks_y_end = zoomstr->zoom * (zoomstr->sizexy.y);
		//	snprintf(sugarbuffer1, 500, "ioks_x: %d, oks_x: %d, [%f x %f] ", (int)ioks_x, (int)oks_x, ioks_y, oks_y);
		//	snprintf(sugarbuffer2, 500, "oks_tx: %d,        %d,  %f   %f  ", (int)oks_tx, (int)oks_x, ioks_y, oks_y);
		ImGui::PushID("canvas");

		//ImGui::GetKeyIndex(ImGuiKey imgui_key);
//		ImGui:SetKeyboardFocusHere(0);

		ImGui::ImageButton((void*)my_texture,
			ImVec2((float)my_image_width  * zoomstr->zoom, (float)my_image_height * zoomstr->zoom),
			ImVec2(zoomstr->offsetxy.x + 0.0f, zoomstr->offsetxy.y + 0.0f),
			ImVec2(zoomstr->offsetxy.x + 1.0f, zoomstr->offsetxy.y + 1.0f),
			0	// no padding
		);
		ImGui::PopID();
		draw_list->AddRectFilled(ImVec2(canvas_pos.x + oks_x, canvas_pos.y), ImVec2(canvas_pos.x + oks_x_end, canvas_pos.y + oks_y), IM_COL32(155, 155, 155, 255));
		draw_list->AddRectFilled(ImVec2(canvas_pos.x, canvas_pos.y + oks_y), ImVec2(canvas_pos.x + oks_x_end, canvas_pos.y + oks_y_end), IM_COL32(155, 155, 155, 255));
		ImVec2 mouse_pos_in_canvas = ImVec2(ImGui::GetIO().MousePos.x - canvas_pos.x + ioks_x, ImGui::GetIO().MousePos.y - canvas_pos.y + ioks_y);
		ImVec2 mouse_pos_in_buffer = from_screen_coords_to_buffer(mouse_pos_in_canvas);
		static ImVec2 mouse_pos_in_canvas_begin = mouse_pos_in_canvas;
		//		
//		sprintf(sugarbuffer1, "MPIC [%f x %f]", mouse_pos_in_canvas.x, mouse_pos_in_canvas.y );
//		sprintf(sugarbuffer2, "MPIB [%f, %f] ", mouse_pos_in_buffer.x, mouse_pos_in_buffer.y );
		static ImVec2 myanchorBegin = { 0.0f,0.0f };
		static ImVec2 myanchorFinal = { 1.0f,1.0f };



		// ******************************************************************************************************************* //
		// ******************************************************************************************************************* //
		// ******************************************************************************************************************* //
		// ******************************************************************************************************************* //
/*
		float mx = ImGui::GetIO().MousePos.x;
		float my = ImGui::GetIO().MousePos.y;
		static float ox = 0, oy = 0;
		static float dx = ox = mx = ImGui::GetIO().MousePos.x;
		static float dy = oy = mx = ImGui::GetIO().MousePos.x;
*/
		static bool door1 = false;
		//static bool door2 = false;
		//static bool door3 = false;
		static int amx=0, amy=0;


		if (ImGui::IsKeyPressedMap(ImGuiKey_Space) && ImGui::IsMousePosValid()) {
//		if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space))) {
			if (door1 == false) {
				SDL_GetGlobalMouseState(&amx, &amy);
				door1 = true;	// close the door
			}

			if (door1 == true) {
				SDL_WarpMouseGlobal( amx,amy); // return 0 on success
			}
			float dx = ImGui::GetIO().MouseDelta.x / 250.0f;
			float dy = ImGui::GetIO().MouseDelta.y / 250.0f;
			zoomstr->offsetxy.x += dx;
			zoomstr->offsetxy.y += dy;
			if (zoomstr->offsetxy.x > 1.0f) zoomstr->offsetxy.x = 1.0f;
			if (zoomstr->offsetxy.y > 1.0f) zoomstr->offsetxy.y = 1.0f;
			if (zoomstr->offsetxy.x < 0.0f) zoomstr->offsetxy.x = 0.0f;
			if (zoomstr->offsetxy.y < 0.0f) zoomstr->offsetxy.y = 0.0f;
		}
//		if (ImGui::IsKeyReleased(ImGuiKey_Space) && ImGui::IsMousePosValid()) {		
			
		

		if (g.IO.KeyCtrl && ImGui::IsKeyPressedMap(ImGuiKey_C)) { // ctrl-C
				printf("ctrl-c was pressed\n");
		}
		

		if (ImGui::IsKeyReleased(ImGui::GetKeyIndex(ImGuiKey_Space))) {
			door1 = false;	// open door							
		}





	
//			int kindex = ImGui::GetKeyIndex( ImGuiKey_Space );
//			int         KeyMap[ImGuiKey_COUNT];         // <unset>          // Map of indices into the KeysDown[512] entries array which represent your "native" keyboard state.
		// Pressing TAB activate widget focus
//			g.FocusTabPressed = (g.NavWindow && g.NavWindow->Active && !(g.NavWindow->Flags & ImGuiWindowFlags_NoNavInputs) && !g.IO.KeyCtrl && IsKeyPressedMap(ImGuiKey_Tab));

//			for (int n = 0; n < ImGuiKey_COUNT; n++)
//				IM_ASSERT(g.IO.KeyMap[n] >= -1 && g.IO.KeyMap[n] < IM_ARRAYSIZE(g.IO.KeysDown) && "io.KeyMap[] contains an out of bound value (need to be 0..512, or -1 for unmapped key)");
//			// Perform simple check: required key mapping (we intentionally do NOT check all keys to not pressure user into setting up everything, but Space is required and was only recently added in 1.60 WIP)
//			if (g.IO.ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard)
//				IM_ASSERT(g.IO.KeyMap[ImGuiKey_Space] != -1 && "ImGuiKey_Space is not mapped, required for keyboard navigation.");
//			if (IsKeyPressedMap(ImGuiKey_C)) { g.NavMoveDirLast = (ImGuiDir)((g.NavMoveDirLast + 1) & 3); g.IO.KeysDownDuration[g.IO.KeyMap[ImGuiKey_C]] = 0.01f; }
//			NAV_MAP_KEY(ImGuiKey_Space, ImGuiNavInput_Activate);
//			const bool page_up_held = IsKeyDown(io.KeyMap[ImGuiKey_PageUp]) && !IsActiveIdUsingKey(ImGuiKey_PageUp);
//			if (IsKeyPressed(io.KeyMap[ImGuiKey_PageUp], true))

//			const bool is_cut = ((is_shortcut_key && IsKeyPressedMap(ImGuiKey_X)) || (is_shift_key_only && IsKeyPressedMap(ImGuiKey_Delete))) && !is_readonly && !is_password && (!is_multiline || state->HasSelection());
//		else if (is_shortcut_key && IsKeyPressedMap(ImGuiKey_A))







	
	// ******************************************************************************************************************* //
	// ******************************************************************************************************************* //
	// ******************************************************************************************************************* //
	// ******************************************************************************************************************* //



		bool selectClickPastCondition = false;

		if (mouseStatusCLICKOFF == true && SelectAlphaFlag == true) { // click OFF
			selectClickPastCondition = true;
			myanchorFinal = myanchorBegin = mouse_pos_in_buffer;
			mouseStatusCLICKOFF = false;
			mouseStatusCLICKON = false;
			mouseStatusDRAG = false;
		}


		if (mouseStatusCLICKOFF == true && mouseStatusDRAG == true && SelectAlphaFlag == false) { // click OFF and DRAG on
			myanchorFinal = mouse_pos_in_buffer;
			float asx = myanchorBegin.x - myanchorFinal.x;
			float asy = myanchorBegin.y - myanchorFinal.y;
			float sx = fabs(asx);
			float sy = fabs(asy);
			if (asx >= 0.0f) {   // do swap the x
				float tmp1 = myanchorBegin.x;
				float tmp2 = myanchorFinal.x;
				myanchorBegin.x = tmp2;
				myanchorFinal.x = tmp1;
			}
			if (asy >= 0.0f) {  // do swap the y
				float tmp1 = myanchorBegin.y;
				float tmp2 = myanchorFinal.y;
				myanchorBegin.y = tmp2;
				myanchorFinal.y = tmp1;
			}

			if (sx > 1.0f && sy > 1.0f && NormalModeFlag == true ) {	// add new rectangle
				RectangleItem *ritem = new RectangleItem(myanchorBegin, myanchorFinal, false, false);
				ritem->setZoomStructure(zoomstr);
				ritemHolder.push_back(ritem);
			}

			if (ToggleSelectionFlag == true ) // UNCLICK inside canvas
			{
				// DONE, see if rectangle of myanchorBegin and myanchorFinal is size 0, then no drag happened...
				if (myanchorBegin.x == myanchorFinal.x && myanchorBegin.y == myanchorFinal.y)
				{// no drag happened here, simple click	

					for (int i = 0;i < (int)ritemHolder.size(); i++) {
						RectangleItem *ritem = ritemHolder[i];
						bool insideretval = ritem->calcIsInside(myanchorBegin);
						if (insideretval) {
							ritem->setSelected(!ritem->isSelected());
							break;
						}
					}
				}
				else {
					// DRAG happened, select multiple
					// **************************** //
					// drag action
					ImVec2 myCurrentAnchor = myanchorBegin;
					set_flag_IsAboutToBeInverted(myCurrentAnchor);
					myCurrentAnchor = myanchorFinal;
					set_flag_IsAboutToBeInverted(myCurrentAnchor);
					myCurrentAnchor.x = myanchorFinal.x;
					myCurrentAnchor.y = myanchorBegin.y;
					set_flag_IsAboutToBeInverted(myCurrentAnchor);
					myCurrentAnchor.x = myanchorBegin.x;
					myCurrentAnchor.y = myanchorFinal.y;
					set_flag_IsAboutToBeInverted(myCurrentAnchor);
					set_flag_IsAboutToBeInvertedInsideContainer(myanchorBegin, myanchorFinal);
					set_flag_IsAboutToBeInvertedInsideHalfSlice(myanchorBegin, myanchorFinal);
					for (int i = 0; i < (int)ritemHolder.size(); i++) {
						RectangleItem *ritem = ritemHolder[i];
						if (ritem->IsAboutToBeInverted())
						{
							ritem->setFlagIsAboutToBeInverted(false);						
//							if (ModifyModeFlag == true) {
//								ritem->setHilighted(!ritem->isHilighted());
//							}
							ritem->setSelected(!ritem->isSelected());
						}
					}
				}// DRAG happened, select multiple
				// **************************** //
			} // if (ToggleSelectionFlag == true) // UNCLICK inside canvas


			if (ModifyModeFlag == true && SelectAlphaFlag == false && 
				ritemHolder.size() > 0 && highlightAreaNumber == 5 )
			{	
				RectangleItem *ritem = ritemHolder[ highlightIndexNumber ];
				lastSelectedRectangleIndex = highlightIndexNumber;
				// ...
				ppx = (int)ritem->getBeginRaw().x;
				ppy = (int)ritem->getBeginRaw().y;
				ppwidth = (int)ritem->getWidthRaw();
				ppheight = (int)ritem->getHeightRaw();
				// ...
				highlightAreaNumber = 0;
				highlightIndexNumber = 0;
				//invalidatelastSelectedRectangleIndex();
			}
			mouseStatusDRAG     = false;
			mouseStatusCLICKOFF = false;
			mouseStatusCLICKON  = false;
		}

//		if (mouseStatusCLICKON == true) {}


		float ix = canvas_size.x;
		float iy = canvas_size.y;
//		sprintf(sugarbuffer1, "%.0f x %0.f [%.0f x %0.f]", ix, iy, mouse_pos_in_canvas.x, mouse_pos_in_canvas.y);
		if ((mouse_pos_in_canvas.x <= 1.0f || mouse_pos_in_canvas.y <= 1.0f || mouse_pos_in_canvas.x > ix || mouse_pos_in_canvas.y > iy ) &&
			mouseStatusDRAG == true && !ImGui::IsItemHovered() ) 
		{
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) 
			{
				// ******************************** //
				// released mouse outside of CANVAS
				// ******************************** //
//	 sprintf(sugarbuffer2, "Mouse Released Outside of CANVAS");
				mouseStatusDRAG     = false;	// 
				mouseStatusCLICKOFF = true;		//
				mouseStatusCLICKON  = false;	//
			}
		}
		









		// on hovering...
		if (ImGui::IsItemHovered())			// take care of watching where the mouse is
		{
			if (SelectAlphaFlag == true) {
				ImGui::SetMouseCursor(7);	// change mouse shape when selecting alpha color
			}

			if (ModifyModeFlag == true && SelectAlphaFlag == false)
			{
				for (int i = 0;i < (int)ritemHolder.size(); i++) {
					RectangleItem *ritem = ritemHolder[i];
					bool insideretval = ritem->calcIsInside(mouse_pos_in_buffer);
					if (insideretval) {
						ImGui::SetMouseCursor(2);	// change mouse shape
					}
				}
			}

			bool isclicked = ImGui::IsItemClicked();
			if (isclicked==true && SelectAlphaFlag == true)	// CLICK ON		
			{
				// DONE, pickup color from image and use it.			
				ImVec2 alphaAnchorPosition = mouse_pos_in_buffer;	// buffer coordinates
				selectedAlphaColor = getpixel(loadedAtlas, (int)alphaAnchorPosition.x, (int)alphaAnchorPosition.y);
				criteria = Criteria::anycolor;	// 
				isclicked = false;
			}
			if (isclicked==true && SelectAlphaFlag == false)	// CLICK ON						
			{
				// ******************************** //
				// CLICK ACTION inside CANVAS
				// ******************************** //
				mouseStatusDRAG     = false;
				mouseStatusCLICKOFF = false;
				mouseStatusCLICKON  = true;
				myanchorBegin = mouse_pos_in_buffer;		 // buffer coordinates
				mouse_pos_in_canvas_begin = mouse_pos_in_canvas; // 
				// *** //
				// *** //
				if (ModifyModeFlag == true) {
					for (int i = 0;i < (int)ritemHolder.size(); i++) {
						ritemHolder[i]->setHilighted(false);	// all to false
					}
					highlightAreaNumber = 0;	// begin, assuming is not inside rectangle
					for (int i = 0;i < (int)ritemHolder.size(); i++) {
						RectangleItem *ritem = ritemHolder[i];
						//highlightAreaNumber = 0; // begin, assuming is not inside rectangle
						bool insideretval = ritem->calcIsInside(mouse_pos_in_buffer);						
						if (insideretval) {
							ritem->calc_knobs(ImVec2{ 0,0 }, false);
							ritem->setHilighted(true);	//
							highlightIndexNumber = i;
							lastSelectedRectangleIndex = i;
							highlightAreaNumber = 5;
							ppx = (int)ritem->getBeginRaw().x;
							ppy = (int)ritem->getBeginRaw().y;
							ppwidth = (int)ritem->getWidthRaw();
							ppheight = (int)ritem->getHeightRaw();
							break;
						}
					}
					if (highlightAreaNumber == 0) {	// clicked outside
						for (int i = 0;i < (int)ritemHolder.size(); i++) {ritemHolder[i]->setHilighted(false);}
						invalidatelastSelectedRectangleIndex();
					}
					

				}

				/*
								// click on inside canvas. hilightMode.	Toggle
								if (ModifyModeFlag == true) {
									for (int i = 0;i < (int)ritemHolder.size(); i++) {
										RectangleItem *ritem = ritemHolder[i];
										bool insideretval = ritem->calcIsInside(mouse_pos_in_buffer);
										if (insideretval) {
											ritem->setHilighted(!ritem->isHilighted());
											break;
										}
									}
								}
				*/

			}
			bool mbo = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
			if (mbo) {	// CLICK OFF
				// ******************************** //
				// released mouse inside of CANVAS
				// ******************************** //
				mouseStatusDRAG		= true; // ? ? ?
				mouseStatusCLICKOFF = true;
				mouseStatusCLICKON	= false;
			}

			ImVec2 drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, -1.0f);
			if (ImGui::IsMouseDragging(ImGuiMouseButton_Left, -1.0f) && SelectAlphaFlag == false) { // While Dragging and mouse is selected
				mouseStatusDRAG     = true;
				mouseStatusCLICKOFF = false;
				mouseStatusCLICKON  = true;
				// *** //
				if (MoveSelectedModeFlag == true) { //
					float dx = ImGui::GetIO().MouseDelta.x;
					float dy = ImGui::GetIO().MouseDelta.y;
					// ImVec2 iv = scale_from_screen_coords_to_buffer(ImVec2{ dx,dy });
					for (int i = 0;i < (int)ritemHolder.size(); i++) {
						RectangleItem *ritem = ritemHolder[i];
						if (ritem->isSelected()) {
							ImVec2 iv = ritem->scale_from_screen_coords_to_buffer(ImVec2{ dx,dy });
							ritem->add_coordinate_dxdy(iv);
						}
					}
				}
				// *** //
			} // While Dragging

			// remove square only on normal mode
			if (NormalModeFlag == true && ImGui::IsMouseClicked(1) && !ritemHolder.empty()) { ritemHolder.pop_back(); }
		} //	if (ImGui::IsItemHovered())		


		//
		//
		//
		//

		// **************************** //
		// draw RECTANGLES and TEXT     //
		// **************************** //
		//
		if (SelectAlphaFlag == false) {
			draw_list->PushClipRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), true);
			setSerialNumber(0);
			setSerialNumber2(0);
			for (int i = 0; i < (int)ritemHolder.size(); i++) {
				RectangleItem *ritem = ritemHolder[i];
				ritem->setZoomStructure(zoomstr);

				//static int chosen_color = IM_COL32(0, 0, 0, 255);
				//static float tickerTimer1 = 0.0f;			
//				int greylevel1 = getTimedSinus();
//				int greylevel2 = getTimedCosinus();
				
/*
				int chosen_color2 = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
				Uint8 ru, gu, bu;
				SDL_GetRGB(selectedAlphaColor, loadedAtlas->format, &ru, &gu, &bu);
				int averageGreyValue = ((int)ru + (int)gu + (int)bu) / 3;
				if (averageGreyValue > 128) {
					chosen_color2 = ImColor(0.5f, 0.6f, 0.7f, 1.0f);	// bluish grey color
				}
				else {
					chosen_color2 = ImColor(1.0f, 1.0f, 0.0f, 1.0f);	// yellow color
				}
*/
				int sinus_grey = getTimedSinus();
				int cosinus_grey = getTimedSinus90();


				int chosen_color0 = IM_COL32(sinus_grey, sinus_grey, sinus_grey, 255);			
				int chosen_color1 = IM_COL32(cosinus_grey, cosinus_grey, cosinus_grey, 255);
				int chosen_color2 = IM_COL32(255,255,255, 255);
				int chosen_color3 = IM_COL32(255,255,255, 255);
				int chosen_color4 = IM_COL32(255,255,255, 255);

				if (showFlashFlag) {
					chosen_color2 = IM_COL32(   sinus_grey, sinus_grey, 0, 255);
					chosen_color3 = IM_COL32(0, sinus_grey, cosinus_grey, 255);
					chosen_color4 = IM_COL32(0, cosinus_grey, sinus_grey, 255);
				}
				// DONE, ...


				// DONE, change color according to alpha color, light or dark.
				Uint8 ru, gu, bu;
				SDL_GetRGB(selectedAlphaColor, loadedAtlas->format, &ru, &gu, &bu);
				int averageGreyValue = ((int)ru + (int)gu + (int)bu) / 3;
				if (averageGreyValue > 128) {
//					chosen_color2 = ImColor(0.0f, 0.0f, 1.0f, 1.0f);	// bluish grey color
				}
				else {
//					chosen_color2 = ImColor(1.0f, 1.0f, 0.0f, 1.0f);	// yellow color
				}


				int s1 = 255;
				int s2 = 0;	
				if (showFlashFlag) {				
					s2 = getTimedSinus();
				}
				ritem->draw_rectangle(draw_list, canvas_pos, 2, IM_COL32(s1,s1,s1,255), IM_COL32(s2, s2, s2, 255), showFlashFlag );
				







				// screen coordinates...
				float x1 = canvas_pos.x + ritem->getBegin().x;
				float y1 = canvas_pos.y + ritem->getBegin().y;
				float x2 = canvas_pos.x + ritem->getEnd().x;
				float y2 = canvas_pos.y + ritem->getEnd().y;
				// screen coordinates...
				// ImU32 color_white = ImColor(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
				if (showNumericalFlag == true && ritem->isSelected()) {		// Display Numerical flag
					char *destination = createNewSerialNumber2();
					float cx = (x1 + x2) / 2.0f - (strlen(destination) * 7.0f) / 2.0f;
					float cy = y1 + ritem->getHeight() + 1;
					draw_list->AddText(ImVec2(cx, cy), chosen_color1, destination);
				}

				if (showFilenameFlag == true && ritem->isSelected()) {		// Display Filename flag
					char *destination = createNewSerialNumber(prefilename_str0, postfilename_str1);
					float cx = (x1 + x2) / 2.0f - (strlen(destination) * 7.0f) / 2.0f;
					float cy = (y1 + y2) / 2.0f - 6.0f;
					draw_list->AddText(ImVec2(cx, cy), chosen_color2, destination);
				}
				
				if (showPositionFlag == true) {		// Display Position flag
					char tmpbuffer[22];
					snprintf((char *)&tmpbuffer[0], sizeof(tmpbuffer) - 2, "%dx%d", (int)ritem->getBegin().x, (int)ritem->getBegin().y); // 
					draw_list->AddText(ImVec2(x1 + 3, y1), chosen_color3, (char *)&tmpbuffer[0]);
				}
				if (showSizeFlag == true) {			// Display Size flag
					char tmpbuffer[22];
					snprintf((char *)&tmpbuffer[0], sizeof(tmpbuffer) - 2, "%dx%d", (int)ritem->getWidth(), (int)ritem->getHeight());
					draw_list->AddText(ImVec2(x1 + 3, y1 + 11), chosen_color4, (char *)&tmpbuffer[0]);
				}

			}
			draw_list->PopClipRect();
			
			// screen coordinates
			float x1 = canvas_pos.x + mouse_pos_in_canvas_begin.x - ioks_x;
			float y1 = canvas_pos.y + mouse_pos_in_canvas_begin.y - ioks_y;
			float x2 = canvas_pos.x + mouse_pos_in_canvas.x - ioks_x;
			float y2 = canvas_pos.y + mouse_pos_in_canvas.y - ioks_y;

			if (ToggleSelectionFlag == true && mouseStatusDRAG == true) {
				draw_list->PushClipRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), true);
				// blue rectangle, while dragging, selecting...
				draw_list->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), IM_COL32(055, 155, 255, 255));
				draw_list->PopClipRect();
			}
			if (NormalModeFlag == true && mouseStatusDRAG == true)
			{
				// green rectangle, define rectangle, base mode...
				draw_list->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), IM_COL32(0, 255, 0, 255));
			}
			// draw while dragging mouse
			if (ModifyModeFlag == true && mouseStatusDRAG == true && highlightAreaNumber == 5)
			{
				draw_list->PushClipRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), true);
				RectangleItem *ritem = ritemHolder[ highlightIndexNumber ];
				ritem->draw_rectangle(draw_list, canvas_pos, 1, 0, IM_COL32(255,0,0,255), showFlashFlag );
				float dx = ImGui::GetIO().MouseDelta.x;
				float dy = ImGui::GetIO().MouseDelta.y;
				ImVec2 iv = ritem->scale_from_screen_coords_to_buffer(ImVec2{ dx,dy });
				ritem->add_coordinate_dxdy(iv);
				draw_list->PopClipRect();
			}
		}
		if (selectClickPastCondition == true) {
			selectClickPastCondition = false;
			SelectAlphaFlag = false;	// DONE, wait until release of mouse...
		}
	}
}






// Demonstrate creating a "main" fullscreen menu bar and populating it.
// Note the difference between BeginMainMenuBar() and BeginMenuBar():
// - BeginMenuBar() = menu-bar inside current window (which needs the ImGuiWindowFlags_MenuBar flag!)
// - BeginMainMenuBar() = helper to create menu-bar-sized window at the top of the main viewport + call BeginMenuBar() into it.
//static void ShowExampleAppMainMenuBar()
void dx11_theMenu() 
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(".SaveExtension."))
		{
			if (ImGui::MenuItem("Either one of three:", "", false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("bmp", "")) {
				strncpy(postfilename_str1, "bmp", IM_ARRAYSIZE(postfilename_str1)-1);
			}
			if (ImGui::MenuItem("png", "")) {
				strncpy(postfilename_str1, "png", IM_ARRAYSIZE(postfilename_str1)-1);
			}
			if (ImGui::MenuItem("jpg (quality = 100)", "")) {
				strncpy(postfilename_str1, "jpg", IM_ARRAYSIZE(postfilename_str1)-1);
			}
			ImGui::Separator();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu( currentFile ))
		{
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}


/*
		static int e = 0;
		ImGui::RadioButton("radio a", &e, 0); ImGui::SameLine();
		ImGui::RadioButton("radio b", &e, 1); ImGui::SameLine();
		ImGui::RadioButton("radio c", &e, 2);

*/




// expand a square by reading the pixel bits and discriminating the alpha bits
void expandSquareVerticalRaw(RectangleItem *ritem) {
	float
		x1 = ritem->getBeginRaw().x,
		y1 = ritem->getBeginRaw().y,
		x2 = ritem->getEndRaw().x,
		y2 = ritem->getEndRaw().y;
	float width = ritem->getWidthRaw();
	float height = ritem->getHeightRaw();
//	float offsety = 0.0f;
	// ************************ //
	int maxy = loadedAtlas->h;
	// ************************ //
	// ************************ //
	for (int y = 0;y < maxy; y++) {
		int howmany_transparent = 0, howmany_opaque = 0;
		// DONE, scan whole line and see if all of the line is transparent, then stop, if y1 is zero or negative, stop.
		for (float x = 0.0f; x < width; x += 1.0f) {
			int alpha = getAlphaValue(loadedAtlas, getpixel(loadedAtlas, (int)(x1 + x), (int)y1), criteria, selectedAlphaColor);
			if (alpha < 127) {	// regular transparent
				howmany_transparent++;
			}
			else {	// opaque
				howmany_opaque++;
			}
		} // for x
		if (howmany_opaque == 0 && howmany_transparent >= (int)width) {
			break;
		}
		else {
			y1 -= 1.0f;
			if (y1 < 0.0f) { 
				y1 = 0.0f;
				break; 
			}
		}
	} // for y
	ritem->setBeginRaw(ImVec2(x1, y1));
	// ...
//	y2 = ritem->getEndRaw().y;
	for (int y = 0;y < maxy; y++) {
		int howmany_transparent = 0, howmany_opaque = 0;
		// DONE, scan whole line and see if all of the line is transparent, then stop, if y2 is zero or negative, stop.
		for (float x = 0.0f; x < width; x += 1.0f) {
			int alpha = getAlphaValue(loadedAtlas, getpixel(loadedAtlas, (int)(x1 + x), (int)y2), criteria, selectedAlphaColor);
			if (alpha < 127) {	// regular transparent
				howmany_transparent++;
			}
			else {	// opaque
				howmany_opaque++;
			}
		} // for x
//		printf("%d,%d,%d\n", (int)width, howmany_opaque, howmany_transparent);
		if (howmany_opaque == 0 && howmany_transparent >= (int)width) {
			break;
		}
		else {
			y2 += 1.0f;
			if ((int)y2 > (int)maxy) { 
				y2 = (float) maxy;
				break; 
			}
		}
	} // for y
	ritem->setEndRaw(ImVec2(x2, y2));
	return;
}










// expand a square by reading the pixel bits and discriminating the alpha bits
void expandSquareHorizontalRaw(RectangleItem *ritem) {
	float
		x1 = ritem->getBeginRaw().x,
		y1 = ritem->getBeginRaw().y,
		x2 = ritem->getEndRaw().x,
		y2 = ritem->getEndRaw().y;
	float width = ritem->getWidthRaw();
	float height = ritem->getHeightRaw();
//	float offsety = 0.0f;
	// ************************ //
	int maxx = loadedAtlas->w;
	// ************************ //
	// ************************ //
	for (int x = 0;x < maxx; x++) {
		int howmany_transparent = 0, howmany_opaque = 0;
		// DONE, scan whole line and see if all of the line is transparent, then stop, if x1 is zero or negative, stop.
		for (float y = 0.0f; y < height; y += 1.0f) {
			int alpha = getAlphaValue(loadedAtlas, getpixel(loadedAtlas, (int)(x1), (int)y1 + (int)y ), criteria, selectedAlphaColor);
			if (alpha < 127) {	// regular transparent
				howmany_transparent++;
			}
			else {	// opaque
				howmany_opaque++;
			}
		} // for x
		if (howmany_opaque == 0 && howmany_transparent >= (int)height) {
			break;
		}
		else {
			x1 -= 1.0f;
			if (x1 < 0.0f){
				x1 = 0.0f;	// TEST
				break;
			}
		}
	} // for y
	ritem->setBeginRaw(ImVec2(x1, y1));
	// ... //
	for (int x = 0;x < maxx; x++) {
		int howmany_transparent = 0, howmany_opaque = 0;
		// DONE, scan whole line and see if all of the line is transparent, then stop, if x1 is zero or negative, stop.
		for (float y = 0.0f; y < height; y += 1.0f) {
			int alpha = getAlphaValue(loadedAtlas, getpixel(loadedAtlas, (int)(x2), (int)y1 + (int)y), criteria, selectedAlphaColor);
			if (alpha < 127) {	// regular transparent
				howmany_transparent++;
			}
			else {	// opaque
				howmany_opaque++;
			}
		} // for x
		if (howmany_opaque == 0 && howmany_transparent >= (int)height) {
			break;
		}
		else {
			x2 += 1.0f;
			if ((int)x2 > (int)maxx){
				x2 = (float) maxx;
				break;
			}
		}
	} // for y
	ritem->setEndRaw(ImVec2(x2, y2));
}




void do_hplusplus()
{
	for (int i = 0;i < (int)ritemHolder.size(); i++) {
		RectangleItem *ritem = ritemHolder[i];
		if (ritem->isSelected()) {			// horizontal plus plus
			ImVec2 begin = ritem->getBeginRaw();
			ImVec2 end = ritem->getEndRaw();
			if (begin.x > 0.0f) begin.x -= 1.0f;
			if (end.x > 0.0f)   end.x += 1.0f;
			ritem->setBeginRaw(begin);
			ritem->setEndRaw(end);
		}
	}

}
void do_vplusplus()
{
	for (int i = 0;i < (int)ritemHolder.size(); i++) {
		RectangleItem *ritem = ritemHolder[i];
		if (ritem->isSelected()) {	// vertical plus plus
			ImVec2 begin = ritem->getBeginRaw();
			ImVec2 end = ritem->getEndRaw();
			if (begin.y > 0.0f) begin.y -= 1.0f;
			if (end.y > 0.0f) end.y += 1.0f;
			ritem->setBeginRaw(begin);
			ritem->setEndRaw(end);
		}
	}
}
void do_hminusminus()
{
	for (int i = 0;i < (int)ritemHolder.size(); i++) {
		RectangleItem *ritem = ritemHolder[i];
		if (ritem->isSelected()) {				// horizontal minus minus
			ImVec2 begin = ritem->getBeginRaw();
			ImVec2 end = ritem->getEndRaw();
			if (begin.x > 0.0f) begin.x += 1.0f;
			if (end.x > 0.0f)     end.x -= 1.0f;
			ritem->setBeginRaw(begin);
			ritem->setEndRaw(end);
		}
	}
}
void do_vminusminus()
{
	for (int i = 0;i < (int)ritemHolder.size(); i++) {
		RectangleItem *ritem = ritemHolder[i];
		if (ritem->isSelected()) {				// vertical minus minus
			ImVec2 begin = ritem->getBeginRaw();
			ImVec2 end = ritem->getEndRaw();
			if (begin.y > 0.0f) begin.y += 1.0f;
			if (end.y > 0.0f)     end.y -= 1.0f;
			ritem->setBeginRaw(begin);
			ritem->setEndRaw(end);
		}
	}
}

//IMGUI_API void          ColorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v);
//IMGUI_API void          ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b);




