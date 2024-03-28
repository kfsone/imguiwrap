#include <array>
#include <functional>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <cstdio>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include "imgui_impl_opengl3_loader.h"
#include <GLFW/glfw3.h>

#include "imguiwrap.dear.h"
#include "imguiwrap.h"
#include "imguiwrap.helpers.h"

#include "imgui_internal.h"

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static std::optional<std::pair<int, int>> newSize{};

// glfw_error_callback is an internal callback for logging any errors raised
// by glfw.
static void
glfw_error_callback(int error, const char* description) noexcept
{
    (void) fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// imgui_main initializes an ImGui openGL/glfw backend and then runs
// the passed ImGuiWrapperFn repeatedly until the std::optional it
// returns has a value, which is then returned as the exit code.
int
imgui_main(const ImGuiWrapConfig& config, const ImGuiWrapperFn& mainFn, const ImGuiWrapperInitFn &initFn) noexcept
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (glfwInit() == 0) {
        return 1;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#else
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#endif

#ifdef IMGUI_HAS_VIEWPORT
	glfwWindowHint(GLFW_VISIBLE, static_cast<int>(!config.hideMainWindow_));
	/// TODO: If the main window is hidden, should we bother setting window information if it can't be seen?
#endif

    // Create window with graphics context
    GLFWwindow* window =
        glfwCreateWindow(config.width_, config.height_, config.windowTitle_, nullptr, nullptr);
    if (window == nullptr) {
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(config.enableVsync_ ? 1 : 0);  // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    if (config.keyboardNav_) {
        ImGui::GetIO().ConfigFlags |=
            ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    }

#ifdef IMGUI_HAS_DOCK
	if(config.enableDocking_){
		ImGui::GetIO().ConfigFlags |=
				ImGuiConfigFlags_DockingEnable;  // Enable Docking
	}
#endif

#ifdef IMGUI_HAS_VIEWPORT
	if(config.enableViewport_){
		ImGui::GetIO().ConfigFlags |=
				ImGuiConfigFlags_ViewportsEnable;  // Enable Docking
	}
	ImGui::GetIO().ConfigViewportsNoAutoMerge = !config.enableViewportAutoMerge_;
#endif

    if(config.startDark_){
		ImGui::StyleColorsDark();
	} else {
		ImGui::StyleColorsLight();
	}

    // Setup Platform/Renderer backends
    /// TODO: Needs to be based on cmake config.
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

	// User Init Function
	/// TODO: Double check if there's a better place to call this
	initFn();

    // Main loop
    const auto&        clearColor = config.clearColor_;
    std::optional<int> exitCode{};

    while (!exitCode.has_value() && glfwWindowShouldClose(window) == 0) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui
        // wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main
        // application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main
        // application. Generally you may always pass all inputs to dear imgui, and hide them from
        // your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        exitCode = mainFn();

        // Rendering
        ImGui::Render();

        // NOLINTNEXTLINE(readability-isolate-declaration) input parameters to next call.
        int display_w{0}, display_h{0};
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        // setup the 'clear' background.
        glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w,
                     clearColor.z * clearColor.w, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);

        // finialize the imgui render into draw data, and render it.
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // swap the render/draw buffers so the user can see this frame.
        glfwSwapBuffers(window);

#ifdef IMGUI_HAS_VIEWPORT
		// Update and Render additional Platform Windows
		if((ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0){
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
#endif

        // change the native (host) window size if requested.
        if (newSize.has_value()) {
            glfwSetWindowSize(window, newSize.value().first, newSize.value().second);
            newSize.reset();
        }
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return exitCode.value_or(0);
}

// flagsWindow is a helper for initializing a flag-editing window.
static void
flagsWindow(const char* title, bool* showing, const std::function<void(void)>& impl) noexcept
{
    if (showing != nullptr && !*showing) {
        return;
    }

    constexpr ImGuiWindowFlags editWindowFlags = ImGuiWindowFlags_NoResize |
                                                 ImGuiWindowFlags_NoFocusOnAppearing;
                                                 //ImGuiWindowFlags_AlwaysUseWindowPadding;

    dear::Begin(title, showing, editWindowFlags) && impl;
}

namespace dear
{
    // SetHostWindowSize advises imgui_main that it should change the native (host)
    // window dimensions at the end of the current frame, using the values from the
    // last call during that frame.
    void SetHostWindowSize(int x, int y) noexcept
    {
        // overwrite any unused value from this frame.
        newSize = std::pair(x, y);
    }

    // EditTableFlags presents a window with selections for all the flags available
    // for a table, allowing you to dynamically modify the table's appearance/layout.
    void EditTableFlags(const char* title, bool* showing, ImGuiTableFlags* flags) noexcept
    {
        // List of the sizing flag names.
        struct SizeInfo
        {
            const char*     name;
            ImGuiTableFlags flag;
        };
        static constexpr std::array<SizeInfo, 5> sizes{
            SizeInfo{"Default", 0},
            SizeInfo{"FixedFit", ImGuiTableFlags_SizingFixedFit},
            SizeInfo{"FixedSame", ImGuiTableFlags_SizingFixedSame},
            SizeInfo{"StretchProp", ImGuiTableFlags_SizingStretchProp},
            SizeInfo{"StretchSame", ImGuiTableFlags_SizingStretchSame},
        };

        flagsWindow(title, showing, [=]() noexcept {
            // Drop-boxes first.
            // Sizing is actually a discrete integer value, shifted 13 bits into the flag.
            const int sizeFlag = *flags & ImGuiTableFlags_SizingMask_;
            size_t sizeSelected = 0;
            for (size_t i = 1; i < sizes.size(); i++) {
                if (sizes[i].flag == sizeFlag) {
                    sizeSelected = i;
                }
            }
            dear::Combo("Sizing", sizes[sizeSelected].name) && [&] {
                for (const auto& size : sizes) {
                    if (ImGui::Selectable(size.name)) {
                        *flags = (*flags & ~ImGuiTableFlags_SizingMask_) | size.flag;
                    }
                }
            };

            // Checkboxes.
            ImGui::CheckboxFlags("Resizable", flags, ImGuiTableFlags_Resizable);
            ImGui::CheckboxFlags("Reorderable", flags, ImGuiTableFlags_Reorderable);
            ImGui::CheckboxFlags("Hideable", flags, ImGuiTableFlags_Hideable);
            ImGui::CheckboxFlags("Sortable", flags, ImGuiTableFlags_Sortable);
            ImGui::CheckboxFlags("NoSavedSettings", flags, ImGuiTableFlags_NoSavedSettings);
            ImGui::CheckboxFlags("ContextMenuInBody", flags, ImGuiTableFlags_ContextMenuInBody);
            ImGui::CheckboxFlags("RowBg", flags, ImGuiTableFlags_RowBg);
            ImGui::CheckboxFlags("BordersInnerH", flags, ImGuiTableFlags_BordersInnerH);
            ImGui::CheckboxFlags("BordersOuterH", flags, ImGuiTableFlags_BordersOuterH);
            ImGui::CheckboxFlags("BordersInnerV", flags, ImGuiTableFlags_BordersInnerV);
            ImGui::CheckboxFlags("BordersOuterV", flags, ImGuiTableFlags_BordersOuterV);
            ImGui::CheckboxFlags("NoBordersInBody", flags, ImGuiTableFlags_NoBordersInBody);
            ImGui::CheckboxFlags("NoBordersInBodyUntilResize", flags,
                                 ImGuiTableFlags_NoBordersInBodyUntilResize);
            ImGui::CheckboxFlags("NoHostExtendX", flags, ImGuiTableFlags_NoHostExtendX);
            ImGui::CheckboxFlags("NoHostExtendY", flags, ImGuiTableFlags_NoHostExtendY);
            ImGui::CheckboxFlags("NoKeepColumnsVisible", flags,
                                 ImGuiTableFlags_NoKeepColumnsVisible);
            ImGui::CheckboxFlags("PreciseWidths", flags, ImGuiTableFlags_PreciseWidths);
            ImGui::CheckboxFlags("PadOuterX", flags, ImGuiTableFlags_PadOuterX);
            ImGui::CheckboxFlags("NoPadOuterX", flags, ImGuiTableFlags_NoPadOuterX);
            ImGui::CheckboxFlags("NoPadInnerX", flags, ImGuiTableFlags_NoPadInnerX);
            ImGui::CheckboxFlags("ScrollX", flags, ImGuiTableFlags_ScrollX);
            ImGui::CheckboxFlags("ScrollY", flags, ImGuiTableFlags_ScrollY);
        });
    }

    // EditWindowFlags presents a window with selections for all the flags available
    // for a window, allowing you to dynamically modify the window's appearance/layout.
    void EditWindowFlags(const char* title, bool* showing, ImGuiWindowFlags* flags) noexcept
    {
        flagsWindow(title, showing, [=]() noexcept {
            ImGui::CheckboxFlags("NoTitleBar", flags, ImGuiWindowFlags_NoTitleBar);
            ImGui::CheckboxFlags("NoResize", flags, ImGuiWindowFlags_NoResize);
            ImGui::CheckboxFlags("NoMove", flags, ImGuiWindowFlags_NoMove);
            ImGui::CheckboxFlags("NoScrollbar", flags, ImGuiWindowFlags_NoScrollbar);
            ImGui::CheckboxFlags("NoScrollWithMouse", flags, ImGuiWindowFlags_NoScrollWithMouse);
            ImGui::CheckboxFlags("NoCollapse", flags, ImGuiWindowFlags_NoCollapse);
            ImGui::CheckboxFlags("AlwaysAutoResize", flags, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::CheckboxFlags("NoBackground", flags, ImGuiWindowFlags_NoBackground);
            ImGui::CheckboxFlags("NoSavedSettings", flags, ImGuiWindowFlags_NoSavedSettings);
            ImGui::CheckboxFlags("NoMouseInputs", flags, ImGuiWindowFlags_NoMouseInputs);
            ImGui::CheckboxFlags("MenuBar", flags, ImGuiWindowFlags_MenuBar);
            ImGui::CheckboxFlags("HorizontalScrollbar", flags,
                                 ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::CheckboxFlags("NoFocusOnAppearing", flags, ImGuiWindowFlags_NoFocusOnAppearing);
            ImGui::CheckboxFlags("NoBringToFrontOnFocus", flags,
                                 ImGuiWindowFlags_NoBringToFrontOnFocus);
            ImGui::CheckboxFlags("AlwaysVerticalScrollbar", flags,
                                 ImGuiWindowFlags_AlwaysVerticalScrollbar);
            ImGui::CheckboxFlags("AlwaysHorizontalScrollbar", flags,
                                 ImGuiWindowFlags_AlwaysHorizontalScrollbar);
            //ImGui::CheckboxFlags("AlwaysUseWindowPadding", flags,
            //                     ImGuiWindowFlags_AlwaysUseWindowPadding);
            ImGui::CheckboxFlags("NoNavInputs", flags, ImGuiWindowFlags_NoNavInputs);
            ImGui::CheckboxFlags("NoNavFocus", flags, ImGuiWindowFlags_NoNavFocus);
            ImGui::CheckboxFlags("UnsavedDocument", flags, ImGuiWindowFlags_UnsavedDocument);
        });
    }

    // EditWindowFlags presents a window for selecting text input field flags.
    void EditInputTextFlags(const char* title, bool* showing, ImGuiInputTextFlags* flags) noexcept
    {
        flagsWindow(title, showing, [=]() noexcept {
            ImGui::CheckboxFlags("CharsDecimal", flags, ImGuiInputTextFlags_CharsDecimal);
            ImGui::CheckboxFlags("CharsHexadecimal", flags, ImGuiInputTextFlags_CharsHexadecimal);
            ImGui::CheckboxFlags("CharsUppercase", flags, ImGuiInputTextFlags_CharsUppercase);
            ImGui::CheckboxFlags("CharsNoBlank", flags, ImGuiInputTextFlags_CharsNoBlank);
            ImGui::CheckboxFlags("AutoSelectAll", flags, ImGuiInputTextFlags_AutoSelectAll);
            ImGui::CheckboxFlags("EnterReturnsTrue", flags, ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::CheckboxFlags("CallbackCompletion", flags,
                                 ImGuiInputTextFlags_CallbackCompletion);
            ImGui::CheckboxFlags("CallbackHistory", flags, ImGuiInputTextFlags_CallbackHistory);
            ImGui::CheckboxFlags("CallbackAlways", flags, ImGuiInputTextFlags_CallbackAlways);
            ImGui::CheckboxFlags("CallbackCharFilter", flags,
                                 ImGuiInputTextFlags_CallbackCharFilter);
            ImGui::CheckboxFlags("AllowTabInput", flags, ImGuiInputTextFlags_AllowTabInput);
            ImGui::CheckboxFlags("CtrlEnterForNewLine", flags,
                                 ImGuiInputTextFlags_CtrlEnterForNewLine);
            ImGui::CheckboxFlags("NoHorizontalScroll", flags,
                                 ImGuiInputTextFlags_NoHorizontalScroll);
            ImGui::CheckboxFlags("AlwaysOverwrite", flags, ImGuiInputTextFlags_AlwaysOverwrite);
            ImGui::CheckboxFlags("ReadOnly", flags, ImGuiInputTextFlags_ReadOnly);
            ImGui::CheckboxFlags("Password", flags, ImGuiInputTextFlags_Password);
            ImGui::CheckboxFlags("NoUndoRedo", flags, ImGuiInputTextFlags_NoUndoRedo);
            ImGui::CheckboxFlags("CharsScientific", flags, ImGuiInputTextFlags_CharsScientific);
            ImGui::CheckboxFlags("CallbackResize", flags, ImGuiInputTextFlags_CallbackResize);
            ImGui::CheckboxFlags("CallbackEdit", flags, ImGuiInputTextFlags_CallbackEdit);
        });
    }

}  // namespace dear
