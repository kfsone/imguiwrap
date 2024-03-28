#pragma once

// define IMGUI_NOEXCEPT as 'noexcept' to enable noexcept behavior (requires compatible imgui.h)
#ifndef IMGUI_NOEXCEPT
#    define IMGUI_NOEXCEPT
#endif

#include "imgui.h"

#include <functional>
#include <optional>

using ImGuiWrapperReturnType = std::optional<int>;
using ImGuiWrapperFn         = std::function<ImGuiWrapperReturnType()>;
using ImGuiWrapperInitFn     = std::function<void()>;

// ImGuiWrapConfig describes the parameters of the main window created by imgui_main.
struct ImGuiWrapConfig
{
    // windowTitle_ sets the initial name given to the main window.
    const char* windowTitle_{"Application"};

    // width_ and height_ control the initial dimensions of the main window.
    template<typename T>
    static constexpr T DefaultWidth = 1280;
    template<typename T>
    static constexpr T DefaultHeight = 720;

    int width_{DefaultWidth<int>}, height_{DefaultHeight<int>};

    // clearColor_ determines the clear/background color for the window.
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    ImVec4 clearColor_{0.45F, 0.55F, 0.60F, 1.00F};

    // enableVsync_ enables vsync for the window.
    bool enableVsync_;

    // keyboardNav_ enables keyboard controls per ImGuiConfigFlags_NavEnableKeyboard;
    bool keyboardNav_{true};

    // startDark_ enables StyleColorsDark after creating the window.
    bool startDark_{true};

#ifdef IMGUI_HAS_DOCK
	// enableDocking_ enables windows to dock with each other
	bool enableDocking_{false};
#endif

#ifdef IMGUI_HAS_VIEWPORT
	// enableViewport_ enables windows to be pulled out of the main window and exist independently
	bool enableViewport_{false};

	// enableViewportAutoMerge_ lets windows merge with the main window if dragged within its bounds.
	// Disabling this forces the windows to be independent
	bool enableViewportAutoMerge_{true};

	// hideMainWindow_ sets the main window to be invisible.
	// This is only useful if enableViewportAutoMerge_ is false as otherwise nothing is displayed
	/// TODO: Perhaps enforce this in code somewhere?
	bool hideMainWindow_{false};
#endif
};

// imgui_main implements a main-loop that constructs a GL window and calls the supplied
// mainFn every frame until the app is closed.
// See dear::SetHostWindowSize if your callback needs to change the GL window size.
extern int imgui_main(const ImGuiWrapConfig& config, const ImGuiWrapperFn& mainFn, const ImGuiWrapperInitFn &initFn = []{}) noexcept;
