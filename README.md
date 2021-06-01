# ImGui Wrappings

This is a trifold wrapper for the [Dear ImGui](https://github.com/ocornut/imgui) library.

1. Ease integration with CMake,
2. Provide an RAII mechanism for ImGui scopes,
3. Provide some minor ImGui helpers,

The RAII mechanisms are written to provide a zero-cost abstraction so that using them
will produce the same machine code (or better) as hand-writing your Begin/End calls.

[godbolt example](https://gcc.godbolt.org/z/saeejdaj4)

![main branch win/lin/mac build](https://github.com/kfsone/imguiwrap/actions/workflows/cmake.yml/badge.svg)

## Integration with your CMakeLists-based project:

The imgui library is exposed as a target: imguiwrap. You can then either manually vendor
or git-submodule imguiwrap into a subdirectory and including imgui in your project is
then as simple as:

```cmake
    add_subdirectory(imguiwrap)
    target_link_libraries(
        YOUR_TARGET

        PUBLIC

        imguiwrap
    )
```

Alternatively, you can use FetchContent or CPM.

## RAII ImGui scopes:

ImGui is essentially a sort of bare-bones virtual machine where you push directives and
parameters onto a stack for execution by the vm. Each directive starts with a push (`Begin`)
of some form and is completed with a corresponding Pop (`End`).

While this isn't particularly difficult, it's a high cognitive load when the surrounding code
is C++.

ImGuiWrap uses zero-cost conceptual classes to provide an RAII approach to ImGui. The constructors
call the relevant `Begin()` function, allow you to execute your dependent code, and then 
conditionally invoke the appropriate `End()` function.

You can do this in a completely conventional way:

```c++
    dear::MainMenuBar bar("Main Menu");
    if (bar) {
        dear::Menu file("File");
        if (file) {
            ...
        }
    }
```

but the classes also implement an `operator&&` which accepts a callable so that you can use them
anonymously and in an increasingly modern compositional style:

```c++
    dear::MainMenuBar("Main Menu")  &&  [](){
        dear::Menu("File")  &&  [](){
            ...
        };
    };
```

(This form might, for instance, look familiar to [boost/ut](https://github.com/boost-ext/ut) users)

And yes, it knows that `ImGui::Begin()` must always have a matching `ImGui::End` while
`ImGui::BeginMainMenuBar` only needs `ImGui::EndMainMenuBar` if the begin returned true.


*Note*
- You may sometimes need to use capturing lambdas, e.g. `[&] {}`,
- If you have access to C++23 you can abbreviate your lambdas from `[] () {...}` to `[] { ... }`,
- You can also use the name of a function:

```c++
    void main_menu() noexcept {
        dear::Menu("File") && [](){
        };
    }

    dear::MainMenuBar("Main Menu") && main_menu;
```

## Minor helpers:

### dear::ItemTooltip

Provides a scoped wrapper that will only execute your callable if the previous item is hovered.

```c++
    dear::Text("[help]");
    dear::ItemTooltip(/*flags*/) && []() { dear::Text("Help is not available"); }
```

### dear::EditTableFlags and EditWindowFlags

These two functions let you edit window or table flags in real time, to help you find
the right flags for your own layouts.

```c++
static ImGuiWindowFlags mywindow_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysVerticalScrollbar;
static bool mywindow_visible  = false;
static bool mywindow_editable = false;

void debug_menu()
{
    dear::Menu("Debug") && []() {
        // Only enable this option if mywindow is being shown.
        dear::MenuItem("Edit MyWindow flags", NULL, &mywindow_editable, mywindow_visible));
    };
}

void show_my_window()
{
    debug_menu();
    dear::Begin("My Window", &mywindow_visible, mywindow_flags) && []() {
        dear::Text("Hello!");
    };
}
```

### dear::Text specialiations

`dear::Text` (and TextUnformatted) specializes for `std::string` and `std::string_view`, which
can be disabled by defining `DEAR_NO_STRING` and `DEAR_NO_STRINGVIEW` accordingly.

It also allows you to avoid the `vsnprintf` overhead of `ImGui::Text` by taking variadic
parameters:

```c++
    ImGui::Text("hello, %s!", "world");  // goes through vsnprintf equiv
    dear::Text("hello, %s!", "world");   // uses perfect-forwarding
```

### dear::MenuItem specializations

`dear::MenuItem` can take a `std::string` as its first argument instead of a `const char*`.

### dear::Zero

Because life is too short to be writing `ImVec2(0, 0)` all over the place...

## DEFER

If crazy RAII operator&& is too much for you, `imguiwrap.helpers.h` provides a simpler
`DEFER` macro too:

```cpp
    DEFER(ImGui::End(););
    if (ImGui:Begin("my window")) {
        if (ImGui::BeginChild("child")) {
            DEFER(ImGui::EndChild(););
        }
    }
```

# Questions

## Why "&&"?

To emphasize that the callable will only be invoked *if* the element is
being rendered.

The approach was inspired by [Boost μt](https://github.com/boost-ext/ut)'s style
of writing unit tests:

```cpp
	"life"_test = [] {
		int i = 43;
		expect(42_i == i);
	};
```

and I seriously considered

```cpp
	"File"_Menu = [] {
		"Open"_MenuItem = onOpen;
		...
	};
```

but the model breaks down for no- and multi-argument cases and I wanted something consistent.

Ultimately while I was reading a mock-up line, I found myself saying "then" so it was either
`&&`, `>>` or `<<`. Connotations imbued by `iostreams` into both of the latter made `>>` feel
very akward while `<<` was less akward but less obviously conditional.

```cpp
	MainMenuBar() << [] { Menu(get_filename(argv[0])); };
	// vs
	MainMenuBar() && [] { Menu(get_filename(argv[0])); };
```

The short-circuit, when the menu bar is not being displayed, is far more obvious in the second form.



## How do the RAII types work?

Each type is a *concrete instantiation* of a 
[CRTP template](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
(so they shouldn't blow up your compilation time). This template, `ScopeWrapper`,
hosts a boolean used to determine if End() needs calling, which can be overwridden
with a template parameter.

These are leveraged in such a way that the compilers can easily recognize that the
bool is unused and eliminate it.

In the following piece of code:

```cpp
    dear::MenuBar("File") && []() {
    };
```

we are (1) constructing a temporary and passing a (3) lambda to an operator method on it
(2) before the object destructs (4).

```cpp
    dear::MenuBar("File")
    ^^^^^^^^^-1-^^^^^^^^^
                          && 
                         ^-2-^
                               [](){ }
                               ^^-3-^^
                                       ;
                                     ^-4-^
```

To expand this out:

```cpp
    {
        dear::MenuBar temp("File");    // temp.ok_ = ImGui::BeginMenuBar("File");

        auto noop_lambda = [] () {};

        temp.operator&&(noop_lambda);  // if (temp.ok_) noop_lambda();

    } // invokes temp.~MenuBar();      -> if (temp.ok_) { ImGui::EndMenuBar(); }
```


# Docker build

There is a `Dockerfile` and `docker-build.sh` provided which I use to test the Linux
build.

    > docker pull kfsone/imguibuild
    or
    > docker build --tag kfsone/imguibuild
    > docker run --rm -it -v ${pwd}:/src kfsone/imguibuild
    > docker-build/example/dear_example
