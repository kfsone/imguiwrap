// Simple demonstration focused on showing the use of imgui_main.

#include "imguiwrap.dear.h"

ImGuiWrapperReturnType
my_render_function()
{
    bool show_window{true};
    dear::Begin("Subwindow", &show_window) && []() {
        dear::Text("Hello, world!");
    };
    // Return a concrete value to exit the loop.
    if (!show_window)
        return 0;
    // Return nothing to continue the loop.
    return {};
}

int
main(int, const char**)
{
    // Passing an entirely-defaulted config, will give us the window title "Application"
    // and the default window configuration.
    return imgui_main(ImGuiWrapConfig{}, my_render_function);
}
