#include <array>

#include "imguiwrap.h"
#include "imguiwrap.helpers.h"
#include "imguiwrap.dear.h"

static void glfw_error_callback(int error, const char* description) noexcept
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int imgui_main(int, char**, ImGuiWrapperFn mainFn) noexcept
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
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
    bool err = gladLoadGL(glfwGetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    ///TODO: Needs to be based on cmake config.
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	std::optional<int> exitCode {};

    // Main loop
    while (!exitCode.has_value() && !glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
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
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
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

static void flagsWindow(const char* title, bool* showing, std::function<void(void)> impl) noexcept
{
	if (showing && !*showing)
		return;

	constexpr ImGuiWindowFlags editWindowFlags =
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_AlwaysUseWindowPadding;

	dear::Begin(title, showing, editWindowFlags) && impl;
}

namespace dear
{

void
EditTableFlags(const char* title, bool* showing, ImGuiTableFlags* flags) noexcept
{
	flagsWindow(title, showing, [=] () noexcept {
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
        int sizing = (*flags & (7 << 13)) >> 13;
        std::array<const char*, 5> sizes = { "Default", "FixedFit", "FixedSame", "StretchProp", "StretchSame" };
        dear::Combo("Sizing", sizes[sizing]) && [&] {
            for (int i = 0; i < sizes.size(); i++) {
                if (ImGui::Selectable(sizes[i]))
                    sizing = i;
            }
        };
        *flags &= ~(7 << 13);
        *flags |= sizing << 13;
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

void
EditWindowFlags(const char* title, bool* showing, ImGuiWindowFlags* flags) noexcept
{
    flagsWindow(title, showing, [=] () noexcept {
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

}

