#pragma once

#include <functional>

#include "imgui.h"

namespace dear
{


// scoped_effect is a helper that uses automatic object lifetime to control
// the invocation of a callable after potentially calling additional code,
// allowing for easy inline creation of scope guards.
//
// On its own, it does nothing but call the supplied function when it is
// destroyed;
struct scoped_effect
{
    using code_t = std::function<void(void)>;

protected:
    const code_t atEnd_;
    const bool ok_;
    const bool mustClose_;

public:
    // constructor takes a predicate that may be used to determine if
    // additional calls can be made, and a function/lambda/callable to
    // be invoked from the destructor.
    scoped_effect(bool ok, code_t atEnd, bool mustClose=false) noexcept : ok_{ok}, atEnd_{atEnd}, mustClose_{mustClose} {}

    // destructor always invokes the supplied destructor function.
    ~scoped_effect() noexcept {
        if (ok_ || mustClose_) atEnd_();
    }

    // operator&& will excute 'code' if the predicate supplied during
    // construction was true.
    bool operator&&(code_t code) const
    {
        if (ok_)
            code();
        return ok_;
    }

    operator bool() const noexcept { return ok_; }

protected:
    scoped_effect(const scoped_effect&) = delete;
    scoped_effect& operator=(const scoped_effect&) = delete;
};

struct Begin : public scoped_effect
{
    // Invoke Begin and guarantee that 'End' will be called.
    Begin(const char* title, bool* open=nullptr, ImGuiWindowFlags flags=0)
        : scoped_effect(ImGui::Begin(title, open, flags), ImGui::End, true)
        {}
};

struct TabBar : public scoped_effect
{
    TabBar(const char* name, ImGuiTabBarFlags flags=0) : scoped_effect(ImGui::BeginTabBar(name, flags), ImGui::EndTabBar) {}
};

struct TabItem : public scoped_effect
{
    TabItem(const char* name, bool* open=nullptr, ImGuiTabItemFlags flags=0) : scoped_effect(ImGui::BeginTabItem(name, open, flags), ImGui::EndTabItem) {}
};

}