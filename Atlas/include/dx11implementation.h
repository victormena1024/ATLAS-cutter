#ifndef DX11IMPLEMENTATION_H
#define DX11IMPLEMENTATION_H

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>

// kissen

void dx11implementation_init( char *filename_input );
bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);


bool dx11_theButtonWindow();
void dx11_theToolWindow();
void dx11_theMenu();
bool dx11_exitNow();



#endif

