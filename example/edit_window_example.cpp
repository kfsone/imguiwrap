#include "imguiwrap.dear.h"

#include <string_view>

// Forward declaration of the main rendering function so that main can appear
// before it in this example.
ImGuiWrapperReturnType my_render_fn();

// imguiwrap provides an optional "imgui_main" function that will setup ImGui and
// then repeatedly invoke your specified function/callable until that returns a
// std::optional<int> with a value.
int main(int argc, char **argv)
{
    return imgui_main(argc, argv, my_render_fn);
}

// This variable decides whether to show the "Window Flag" edit window.
static bool editWindow1{true};
static bool editWindow2{false};
static bool editTable1{true};

ImGuiWrapperReturnType
my_render_fn()
{
    // Create the main menu.
    dear::MainMenuBar() && []() {    // Begin a MenuBar and execute the code in this scope if the bar is rendered.
        dear::Menu("File") && []() { // Begin a Menu and execute the code in this scope if the menu is rendered.
            bool quitting{false};
            dear::MenuItem("Exit", &quitting);
            if (quitting)
                exit(0);
            dear::ItemTooltip() && []()
            {
                dear::Text("Quit the application");
            };
        };
        dear::Menu("Debug") && []()
        {
            dear::MenuItem("Edit Window 1", &editWindow1);
            dear::MenuItem("Edit Window 2", &editWindow2);
            dear::MenuItem("Edit Table 1", &editTable1);
        };
    };

    // Allow the user to optionally edit the flags for the window.
    static ImGuiWindowFlags window1Flags{ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize};
    dear::EditWindowFlags("Flags for Window 1", &editWindow1, &window1Flags);
    // Start a new window.
    dear::Begin("Window 1", nullptr, window1Flags) && []()
    {
        static constexpr size_t BoardWidth = 3, BoardHeight = 3;
        static std::string board[BoardWidth * BoardHeight];
        static bool initialized{false};
        if (dear::Selectable("Reset") || !initialized)
        {
            std::fill(std::begin(board), std::end(board), ".");
            initialized = true;
        }
        static ImGuiTableFlags table1Flags{ImGuiTableFlags_Borders};
        dear::EditTableFlags("Flags for Table 1", &editTable1, &table1Flags);
        dear::Table("0s and Xs", BoardWidth, table1Flags) && []()
        {
            // Imgui's table will automatically wrap for us.
            for (auto &it : board)
            {
                if (ImGui::TableNextColumn())
                {
                    if (it == ".")
                    {
                        dear::Selectable(it);
                        if (ImGui::IsItemHovered())
                        {
                            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                                it = "0";
                            else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
                                it = "X";
                        }
                    }
                    else
                        dear::Text(it);
                }
            }
        };
    };

    // Returns "no value" (see std::optional), allowing the loop to continue
    return {};
}
