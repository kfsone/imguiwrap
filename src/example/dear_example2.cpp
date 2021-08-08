#include "imguiwrap.dear.h"

#include <array>
#include <string>

ImGuiWrapperReturnType
my_render_fn()
{
    bool quitting{false};
    dear::Begin("Window 1") && [&quitting]() {
        dear::Text("This is window 1");
        dear::Selectable("Click me to quit", &quitting);
    };
    if (quitting)
        return 0;

    dear::Begin("Window 2", nullptr, ImGuiWindowFlags_AlwaysAutoResize) && []() {
        static constexpr size_t                             boarddim = 3;
        static std::array<std::string, boarddim * boarddim> board{"X", "O", "O", "O", "X",
                                                                  "O", "O", "X", " "};
        dear::Table("0s and Xs", 3, ImGuiTableFlags_Borders) && []() {
            for (const auto& box : board) {
                ImGui::TableNextColumn();
                dear::Text(box);
            }
        };
    };
    return {};
}

// imguiwrap provides an optional "imgui_main" function that will setup ImGui and
// then repeatedly invoke your specified function/callable until that returns a
// std::optional<int> with a value.
int
main(int, const char**)
{
#if __cplusplus__ >= 202000ULL
    ImGuiWrapConfig config{
        .windowTitle_ = "Basic Example",
        .width_       = 1280,
        .height_      = 600,
        .clearColor_  = ImVec4{0.4f, 0.6f, 0.5f, 1.0f},
        .enableVsync_ = false,  // default is true
        .keyboardNav_ = false,  // default is true
        .startDark_   = false,  // default is true
    };
#else
    ImGuiWrapConfig config{
        "Basic Example", 1280, 640, ImVec4{0.4f, 0.6f, 0.5f, 1.0f},
        false,  // default is true
        false,  // default is true
        false,  // default is true
    };
#endif

    return imgui_main(config, my_render_fn);
}
