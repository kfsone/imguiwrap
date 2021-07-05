#include <array>
#include <functional>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <cstdio>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h>            // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

#include "imguiwrap.dear.h"
#include "imguiwrap.h"
#include "imguiwrap.helpers.h"

#include "imgui_internal.h"

// glfw_error_callback is an internal callback for logging any errors raised
// by glfw.
static void
glfw_error_callback(int error, const char* description) noexcept
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// imgui_main initializes an ImGui openGL/glfw backend and then runs
// the passed ImGuiWrapperFn repeatedly until the std::optional it
// returns has a value, which is then returned as the exit code.
int
imgui_main(ImGuiWrapperFn mainFn) noexcept
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

        // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.2 + GLSL 130
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui Test", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
    bool err = gladLoadGL(glfwGetProcAddress) ==
               0;  // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
    bool err = false;  // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of
                       // initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    /// TODO: Needs to be based on cmake config.
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    std::optional<int> exitCode{};

    // Main loop
    while (!exitCode.has_value() && !glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your
        // inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those
        // two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        exitCode = mainFn();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w,
                     clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
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
flagsWindow(const char* title, bool* showing, std::function<void(void)> impl) noexcept
{
    if (showing && !*showing)
        return;

    constexpr ImGuiWindowFlags editWindowFlags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysUseWindowPadding;

    dear::Begin(title, showing, editWindowFlags) && impl;
}

namespace dear
{

// EditTableFlags presents a window with selections for all the flags available
// for a table, allowing you to dynamically modify the table's appearance/layout.
void
EditTableFlags(const char* title, bool* showing, ImGuiTableFlags* flags) noexcept
{
    // List of the sizing flag names.
    struct SizeInfo { const char* name; ImGuiTableFlags flag; };
    static constexpr std::array<SizeInfo, 5> sizes {
        SizeInfo{ "Default", 0 },
        SizeInfo{ "FixedFit", ImGuiTableFlags_SizingFixedFit },
        SizeInfo{ "FixedSame", ImGuiTableFlags_SizingFixedSame },
        SizeInfo{ "StretchProp", ImGuiTableFlags_SizingStretchProp },
        SizeInfo{ "StretchSame", ImGuiTableFlags_SizingStretchSame },
    };

    flagsWindow(title, showing, [=]() noexcept {
        // Drop-boxes first.
        // Sizing is actually a discrete integer value, shifted 13 bits into the flag.
        int sizeFlag = *flags & ImGuiTableFlags_SizingMask_;
        size_t sizeSelected = 0;
        for (size_t i = 1; i < sizes.size(); i++)
            if (sizes[i].flag == sizeFlag)
                sizeSelected = i;
        dear::Combo("Sizing", sizes[sizeSelected].name) && [&] {
            for (size_t i = 0; i < sizes.size(); i++)
            {
                if (ImGui::Selectable(sizes[i].name))
                    *flags = (*flags & ~ImGuiTableFlags_SizingMask_) | sizes[i].flag;
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
        ImGui::CheckboxFlags("NoBordersInBodyUntilResize", flags, ImGuiTableFlags_NoBordersInBodyUntilResize);
        ImGui::CheckboxFlags("NoHostExtendX", flags, ImGuiTableFlags_NoHostExtendX);
        ImGui::CheckboxFlags("NoHostExtendY", flags, ImGuiTableFlags_NoHostExtendY);
        ImGui::CheckboxFlags("NoKeepColumnsVisible", flags, ImGuiTableFlags_NoKeepColumnsVisible);
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
void
EditWindowFlags(const char* title, bool* showing, ImGuiWindowFlags* flags) noexcept
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
        ImGui::CheckboxFlags("HorizontalScrollbar", flags, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::CheckboxFlags("NoFocusOnAppearing", flags, ImGuiWindowFlags_NoFocusOnAppearing);
        ImGui::CheckboxFlags("NoBringToFrontOnFocus", flags, ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::CheckboxFlags("AlwaysVerticalScrollbar", flags, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        ImGui::CheckboxFlags("AlwaysHorizontalScrollbar", flags, ImGuiWindowFlags_AlwaysHorizontalScrollbar);
        ImGui::CheckboxFlags("AlwaysUseWindowPadding", flags, ImGuiWindowFlags_AlwaysUseWindowPadding);
        ImGui::CheckboxFlags("NoNavInputs", flags, ImGuiWindowFlags_NoNavInputs);
        ImGui::CheckboxFlags("NoNavFocus", flags, ImGuiWindowFlags_NoNavFocus);
        ImGui::CheckboxFlags("UnsavedDocument", flags, ImGuiWindowFlags_UnsavedDocument);
    });
}

// _text_impl is a helper for writing Text trampolines for non-c-strings.
void
_text_impl(std::function<const char*(char*, size_t)> formatter) noexcept
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    ImGui::TextEx(g.TempBuffer, formatter(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer)),
                  ImGuiTextFlags_NoWidthForLargeClippedText);
}

}  // namespace dear
