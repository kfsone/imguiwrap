// Simple demonstration focused on showing the use of imgui_main.

#include "imguiwrap.dear.h"

ImGuiWrapperReturnType
my_render_function()
{
    bool show_window { true };
    dear::Begin("Subwindow", &show_window) && [](){
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
    // Passing an ImGuiWrapConfig with just our custom title.
    return imgui_main(ImGuiWrapConfig{"Hello World Example"}, my_render_function);
}
