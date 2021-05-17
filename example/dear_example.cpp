/* Demonstration of using the imguiwrapper "dear::" functions.

    imguiwrap.dear.h provides a set of helpers in the "dear" namespace,
    which invoke the appropriate "Begin" method and ensure that the
    relevant "End" function is called after executing a given block of code.

    Each works as follows:

        dear::<name>(<arguments>) && <callable>;

    In principle this creates a temporary object which calls the Begin...
    function with those arguments, and if that returns true, invokes the
    callable. Finally, in the temporary's destructor, the relevant End...
    is called.

    Compilers can typically eliminate the temporary.

    Callable can be a function or a lambda.

    Thus

        void about_tab() { ImGui::Text("I made this."); }
        void bars() {
            dear::TabItem("About") && about_tab;
            dear::TabItem("Help") && [] { ImGui::Text("No help for you."); };
        }

    is equivalent to

        void bars() {
            if (ImGui::BeginTabItem("About")) {
                ImGui::Text("I made this");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Help")) {
                ImGui::Text("No help for you.");
                ImGui::EndTabItem();
            }
        }
*/

#include "imguiwrap.helpers.h"
#include "imguiwrap.dear.h"

#include <string_view>

ImGuiWrapperReturnType
windowFn()
{
    static bool show_visualizer { true };
	static bool selected { false };

    // Returning a value will translate to an exit code.
    if (!show_visualizer)
        return 0;

	static bool is_quitting;
    dear::Begin("Visualizer", &show_visualizer) && [] {
		dear::MainMenuBar() && [] {
			dear::Menu("File") && [] {
				ImGui::MenuItem("Wibble", NULL, &selected);
				ImGui::MenuItem("BOOM");
			};
			dear::Menu("Quitters") && [] {
				is_quitting = ImGui::MenuItem("QUIT NAOW");
			};
		};
        ImGui::Text("hello");
        dear::TabBar("##TabBar") && [] {
            dear::TabItem("Files") && [] {
                ImGui::Text("...files...");
            };
            dear::TabItem("Blueprints") && [] {
                ImGui::Text("...blueprints...");
            };
            dear::TabItem("Enums") && [] {
                ImGui::Text("...enums...");
            };
            dear::TabItem("Prototypes") && [] {
                ImGui::Text("...prototypes...");
            };
			dear::TabItem("More") && [] {
				dear::Child("hello") && [] {
					dear::Group() && [] {
						dear::Combo("me", "you") && [] {
							dear::Tooltip() && [] {
								ImGui::SetTooltip("You are now viewing a tooltip...");
							};
						};
						dear::ListBox("list of things");
					};
				};
			};
        };
    };
	if (is_quitting)
		return 0;

    // Returns "no value" (see std::optional)
    return {};
}

int
main(int argc, char** argv)
{
    return imgui_main(argc, argv, windowFn);
}
