#include "imguiwrap.dear.h"

#include <array>
#include <string>
#include <string_view>

// Forward declaration of the main rendering function so that main can appear
// before it in this example.
ImGuiWrapperReturnType my_render_fn();

// imguiwrap provides an optional "imgui_main" function that will setup ImGui and
// then repeatedly invoke your specified function/callable until that returns a
// std::optional<int> with a value.
int
main(int, const char**)  // NOLINT(readability-named-parameter)
{
    const ImGuiWrapConfig config{};  // Using defaults.
    return imgui_main(config, my_render_fn);
}

// This variable decides whether to show the "Window Flag" edit window.
static bool editWindow1{true};
static bool editWindow2{false};
static bool editTable1{true};

// Width and height of the board.
static constexpr size_t BoardDim = 3;

// Board itself.
static std::array<std::string, BoardDim*BoardDim> board{};

ImGuiWrapperReturnType
my_render_fn()
{
	// Controlled by the exit menu item.
	static bool quitting{false};

    // Create the main menu.
    dear::MainMenuBar() &&
        []() {  // Begin a MenuBar and execute the code in this scope if the bar is rendered.
            dear::Menu("File") &&
                []() {  // Begin a Menu and execute the code in this scope if the menu is rendered.
                    dear::MenuItem("Exit", &quitting);

                    dear::ItemTooltip() && []() {
                        dear::Text("Quit the application");
                    };
                };

            dear::Menu("Debug") && []() {
                dear::MenuItem("Edit Window 1", &editWindow1);
                dear::MenuItem("Edit Window 2", &editWindow2);
                dear::MenuItem("Edit Table 1", &editTable1);
            };
        };

    // Allow the user to optionally edit the flags for the window.
    static ImGuiWindowFlags window1Flags{ImGuiWindowFlags_NoCollapse |
                                         ImGuiWindowFlags_AlwaysAutoResize};
    dear::EditWindowFlags("Flags for Window 1", &editWindow1, &window1Flags);
    // Start a new window.
    dear::Begin("Window 1", nullptr, window1Flags) && []() {

		// Ensure we initialize the board at least once.
        static bool             initialized{false};
        if (dear::Selectable("Reset") || !initialized) {
            std::fill(std::begin(board), std::end(board), ".");
            initialized = true;
        }

		// Expose an edit window for the table flags.
        static ImGuiTableFlags table1Flags{ImGuiTableFlags_Borders};
        dear::EditTableFlags("Flags for Table 1", &editTable1, &table1Flags);

		// Draw the game board.
        dear::Table("0s and Xs", BoardDim, table1Flags) && []() {

            for (auto& it : board) {

                if (ImGui::TableNextColumn()) {  // automatically handles rows.

					// If the cell is empty (".") then render it as a clickable,
                    if (it == ".") {
                        dear::Selectable(it);

						// If the mouse is over it, check and see if they've clicked.
                        if (ImGui::IsItemHovered()) {

							// LMB -> 0, RMB -> X
                            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                                it = "0";
							} else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
                                it = "X";
							}
                        }

                    } else {
						// Not empty: render the text now populating it.
                        dear::Text(it);
					}
                }
            }
        };
    };

	// Return a concrete value to terminate.
	if (quitting) {
		return 0;
	}

    // Returns "no value" (see std::optional), allowing the loop to continue
    return {};
}
