#pragma once

// define IMGUI_NOEXCEPT as 'noexcept' to enable noexcept behavior (requires compatible imgui.h)
#ifndef IMGUI_NOEXCEPT
# define IMGUI_NOEXCEPT
#endif

#include "imgui.h"

#include <functional>
#include <optional>

using ImGuiWrapperReturnType = std::optional<int>;
using ImGuiWrapperFn = std::function<ImGuiWrapperReturnType()>;

extern int imgui_main(int, char**, ImGuiWrapperFn mainFn) noexcept;
