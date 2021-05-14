#pragma once

#include "imgui.h"

namespace dear
{


// scoped_effect is a helper that uses automatic object lifetime to control
// the invocation of a callable after potentially calling additional code,
// allowing for easy inline creation of scope guards.
//
// On its own, it does nothing but call the supplied function when it is
// destroyed;
template<typename Base, bool ForceDtor=false>
struct ScopeWrapper
{
    using wrapped_type = Base;
    using self_type = ScopeWrapper<Base>;

    static constexpr bool force_dtor = ForceDtor;

protected:
    const bool ok_;

public:
    // constructor takes a predicate that may be used to determine if
    // additional calls can be made, and a function/lambda/callable to
    // be invoked from the destructor.
    constexpr ScopeWrapper(bool ok) noexcept : ok_{ok} {}

    // destructor always invokes the supplied destructor function.
    constexpr ~ScopeWrapper() noexcept {
        if constexpr (!force_dtor) {
            if (!ok_)
                return;
        }
        Base::dtor();
    }

    // operator&& will excute 'code' if the predicate supplied during
    // construction was true.
    template<typename PassthruFn>
    constexpr bool operator&&(PassthruFn passthru) const
    {
        if (ok_)
            passthru();
        return ok_;
    }

    constexpr operator bool() const noexcept { return ok_; }

protected:
    ScopeWrapper(const ScopeWrapper&) = delete;
    ScopeWrapper& operator=(const ScopeWrapper&) = delete;
};

struct Begin : public ScopeWrapper<Begin, true>
{
    // Invoke Begin and guarantee that 'End' will be called.
    Begin(const char* title, bool* open=nullptr, ImGuiWindowFlags flags=0)
        : ScopeWrapper(ImGui::Begin(title, open, flags))
        {}
    static constexpr void(*dtor)() = ImGui::End;
};

struct TabBar : public ScopeWrapper<TabBar>
{
    TabBar(const char* name, ImGuiTabBarFlags flags=0)
        : ScopeWrapper(ImGui::BeginTabBar(name, flags)) {}
    static constexpr void(*dtor)() = ImGui::EndTabBar;
};

struct TabItem : public ScopeWrapper<TabItem>
{
    TabItem(const char* name, bool* open=nullptr, ImGuiTabItemFlags flags=0)
        : ScopeWrapper(ImGui::BeginTabItem(name, open, flags)) {}
    static constexpr void(*dtor)() = ImGui::EndTabItem;
};

}