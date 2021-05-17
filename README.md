# ImGui Wrappings

Essentially a CMake wrapper around imgui, initially targetting glfw/opengl use.

## Integration with your CMakeLists-based project:

```cmake
    add_subdirectory(imguiwrap)
    target_link_libraries(
        YOUR_TARGET

        PUBLIC

        imguiwrap
    )
```

This will also add include paths and libraries required to `YOUR_TARGET`.

# Usage

In addition to basic ImGui functionality, there are helpers in `imguiwrap.helpers.h`

## DEFER

ImGui usage relies heavily on ensuring you pop things off the ImGui stack in the right
order. To help with this, I've provided a rust/go-style 'defer' macro:

```cpp
    DEFER(ImGui::End(););
    if (ImGui:Begin("my window")) {
        if (ImGui::BeginChild("child")) {
            DEFER(ImGui::EndChild(););
        }
    }
```

## "dear" namespace

Wouldn't it be nice if you didn't have to manually track all those `Begin/End()`s?

The `dear` namespace in `imguiwrap.dear.h` provides helpers for exactly this. These
create temporary objects (which the compiler can eliminate) that implement an `&&`
operator that allows them to (optionally) call code, which can be either a named
function or a lambda.

```cpp
    // how to draw the about tab.
    dear::TabItem("About") && [] {  // note: c++20 lambda
        ImGui::Text("This is the about tab");
    };
```

### Why "&&"

To emphasize that the callable will only be invoked *if* the element is
being rendered.

### Under the hood

What's happening here? Lets translate this into a longer form:

```cpp
    auto drawAboutTab = [] () {
        ImGui::Text("This is the about tab");
    };
    {
        dear::TabItem temporary_object("About");
        temporary_object.operator&&(drawAboutTab);
    }
```

The dear::&& syntax results in a temporary object whose lifetime lasts until the
end of the call to operator&&; if the item is being rendered, then the call to
operator&& will include the execution of the callable.


### Using named functions

Because the argument to `operator&&` is a std::function, it can accept any form
of callable, whether it is in immediate, a function, or a capturing lambda.

```cpp
    void draw_about() { ImGui::Text("This is the about tab."); }

    void my_window_fn()
    {
        dear::TabItem("about") && draw_about;
    }
```
