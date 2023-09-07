#pragma once

#include "external/imgui/imgui_impl_glfw.h"
#include "external/imgui/imgui_impl_opengl3.h"

#ifdef OSLO_IMGUI_IMPL
#include "external/imgui/imgui_impl_glfw.cpp"
#include "external/imgui/imgui_impl_opengl3.cpp"
#endif