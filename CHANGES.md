v1.2.0 2021/08/08
- added dear::SetHostWindowSize: allows `imgui_main` callbacks to alter native window size,
- added dear::EditInputTextFlags: properties editor for input fields,
- added and applied .clang-format
- added .clang-tidy:
-- reorganized imguiwrap code into src/ subdirectory to isolate application of clang-tidy
   (without this, it would try to clang-tidy the vendor'd code too)
-- added clang-tidy config to src/CMakeLists.txt
-- implemented changes/fixes to comply with numerous clang-tidy warnings,

v1.1.0 2021/07/06
- changed `imgui_main` fingerprint:
-- added `ImGuiWrapConfig` for describing your main window for `imgui_main`,
-- removed `argc, argv` since the config parameter zeroes their potential value,
-- updated examples
-- updated documentation,
- updated imgui and glfw
- added a CHANGES file,

