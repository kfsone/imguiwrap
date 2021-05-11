#include "imguiwrap.helpers.h"

#include <string_view>

void windowFn()
{
    static bool show_visualizer { true };
    DEFER(ImGui::End(););
    if (!ImGui::Begin("Visualizer", &show_visualizer))
        return;

    ImGui::Text("hello");
    if (ImGui::BeginTabBar("##TabBar"))
    {
        DEFER(ImGui::EndTabBar(););
        if (ImGui::BeginTabItem("Files"))
        {
            DEFER(ImGui::EndTabItem(););
            ImGui::Text("...files...");
        }
        if (ImGui::BeginTabItem("Blueprints"))
        {
            DEFER(ImGui::EndTabItem(););
            ImGui::Text("...blueprints...");
        }
        if (ImGui::BeginTabItem("Enums"))
        {
            DEFER(ImGui::EndTabItem(););
            ImGui::Text("... enums...");
        }
        if (ImGui::BeginTabItem("Prototypes"))
        {
            DEFER(ImGui::EndTabItem(););
            ImGui::Text("... prototypes...");
        }
    }
}

int
main(int argc, char** argv)
{
    return imgui_main(argc, argv, windowFn);
}
