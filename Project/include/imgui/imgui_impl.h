#pragma once

#include <imgui/imgui.h>
#include <Windows.h>

bool ImGui_Impl_CreateFontsTexture();
bool ImGui_Impl_CreateDeviceObjects();
void ImGui_Impl_NewFrame(int width, int height);
void ImGui_Impl_RenderDrawList(ImDrawData * draw_data);
void ImGui_Impl_InvalidateDeviceObjects();
void ImGui_Impl_Shutdown();
bool ImGui_Impl_Init(HWND hWnd);