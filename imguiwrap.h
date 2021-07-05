#pragma once

// define IMGUI_NOEXCEPT as 'noexcept' to enable noexcept behavior (requires compatible imgui.h)
#ifndef IMGUI_NOEXCEPT
#define IMGUI_NOEXCEPT
#endif

#include "imgui.h"

#include <functional>
#include <optional>

using ImGuiWrapperReturnType = std::optional<int>;
using ImGuiWrapperFn = std::function<ImGuiWrapperReturnType()>;

// ImGuiWrapConfig describes the parameters of the main window created by imgui_main.
struct ImGuiWrapConfig
{
    // windowTitle_ sets the initial name given to the main window.
    const char *windowTitle_{"Application"};

    // width_ and height_ control the initial dimensions of the main window.
    int width_{1280}, height_{720};

    // clearColor_ determines the clear/background color for the window.
    ImVec4 clearColor_{0.45f, 0.55f, 0.60f, 1.00f};

    // enableVsync_ enables vsync for the window.
    bool enableVsync_;

    // keyboardNav_ enables keyboard controls per ImGuiConfigFlags_NavEnableKeyboard;
    bool keyboardNav_{true};

    // startDark_ enables StyleColorsDark after creating the window.
    bool startDark_{true};
};

// imgui_main implements a main-loop that constructs a GL window and calls the supplied
// mainFn every frame until the app is closed.
extern int imgui_main(const ImGuiWrapConfig &config, ImGuiWrapperFn mainFn) noexcept;
