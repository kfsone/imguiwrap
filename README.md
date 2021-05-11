# ImGui Wrappings

Essentially a CMake wrapper around imgui, initially targetting glfw/opengl use.

Use:

    add_subdirectory(imguiwrap)
    target_link_libraries(
        YOUR_TARGET

        PUBLIC

        imguiwrap
    )

This will add include paths and libraries required to your project.
