#include "imguiwrap.dear.h"

#include <array>
#include <string>

ImGuiWrapperReturnType
my_render_fn()
{
  bool quitting { false };
  dear::Begin("Window 1") && [&quitting](){
    dear::Text("This is window 1");
	dear::Selectable("Click me to quit", &quitting);
  };
  if (quitting) return 0;

  dear::Begin("Window 2", nullptr, ImGuiWindowFlags_AlwaysAutoResize) && [](){
    static constexpr size_t boarddim = 3;
    static std::array<std::string, boarddim * boarddim> board { "X", "O", "O", "O", "X", "O", "O", "X", " " };
    dear::Table("0s and Xs", 3, ImGuiTableFlags_Borders) && [](){
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
int main(int, const char **)
{
    return imgui_main(my_render_fn);
}
