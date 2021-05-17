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
    Begin(const char* title, bool* open=nullptr, ImGuiWindowFlags flags=0) noexcept : ScopeWrapper(ImGui::Begin(title, open, flags)) {}
    static constexpr void(*dtor)() IMGUI_NOEXCEPT = ImGui::End;
};

struct Child : public ScopeWrapper<Child, true>
{
	Child(const char* title, const ImVec2& size=ImVec2(0, 0), bool border=false, ImGuiWindowFlags flags=0) noexcept : ScopeWrapper(ImGui::BeginChild(title, size, border, flags)) {}
	Child(ImGuiID id, const ImVec2& size=ImVec2(0, 0), bool border=false, ImGuiWindowFlags flags=0) noexcept : ScopeWrapper(ImGui::BeginChild(id, size, border, flags)) {}
	static constexpr void(*dtor)() IMGUI_NOEXCEPT = ImGui::EndChild;
};

struct Group : public ScopeWrapper<Group>
{
	Group() noexcept : ScopeWrapper(true) { ImGui::BeginGroup(); }
	static constexpr void(*dtor)() IMGUI_NOEXCEPT = ImGui::EndGroup;
};

struct Combo : public ScopeWrapper<Combo>
{
	Combo(const char* label, const char* preview, ImGuiComboFlags flags=0) noexcept : ScopeWrapper(ImGui::BeginCombo(label, preview, flags)) {}
	static constexpr void(*dtor)() IMGUI_NOEXCEPT = ImGui::EndCombo;
};

struct ListBox : public ScopeWrapper<ListBox>
{
	ListBox(const char* label, const ImVec2& size=ImVec2(0, 0)) noexcept : ScopeWrapper(ImGui::BeginListBox(label, size)) {}
	static constexpr void(*dtor)() IMGUI_NOEXCEPT = ImGui::EndListBox;
};

struct MenuBar : public ScopeWrapper<MenuBar>
{
	MenuBar() noexcept : ScopeWrapper(ImGui::BeginMenuBar()) {}
	static constexpr void(*dtor)() IMGUI_NOEXCEPT = ImGui::EndMenuBar;
};

struct MainMenuBar : public ScopeWrapper<MainMenuBar>
{
	MainMenuBar() noexcept : ScopeWrapper(ImGui::BeginMainMenuBar()) {}
	static constexpr void(*dtor)() IMGUI_NOEXCEPT = ImGui::EndMainMenuBar;
};

struct Menu : public ScopeWrapper<Menu>
{
	Menu(const char* label, bool enabled=true) noexcept : ScopeWrapper(ImGui::BeginMenu(label, enabled)) {}
	static constexpr void(*dtor)() IMGUI_NOEXCEPT = ImGui::EndMenu;
};

struct Tooltip : public ScopeWrapper<Tooltip>
{
	Tooltip() noexcept : ScopeWrapper(true) { ImGui::BeginTooltip(); }
	static constexpr void(*dtor)() IMGUI_NOEXCEPT = ImGui::EndTooltip;
};

struct Popup : public ScopeWrapper<Popup>
{
	struct modal {};

	Popup(const char* str_id, ImGuiWindowFlags flags=0) noexcept : ScopeWrapper(ImGui::BeginPopup(str_id, flags)) {}

	// There are 3 ways to construct a modal popup:
	// - Use the PopupModal class,
	// - Use Popup(modal{}, ...)
	// - Use the static method Popup::Modal(...)
	Popup(modal, const char* name, bool* p_open=NULL, ImGuiWindowFlags flags=0) noexcept : ScopeWrapper(ImGui::BeginPopupModal(name, p_open, flags)) {}
	static Popup Modal(const char* name, bool* p_open=NULL, ImGuiWindowFlags flags=0) noexcept { return Popup(modal{}, name, p_open, flags); }

	static constexpr void(*dtor)() IMGUI_NOEXCEPT = ImGui::EndPopup;
};

struct PopupModal : public ScopeWrapper<PopupModal>
{
	PopupModal(const char* name, bool* p_open=NULL, ImGuiWindowFlags flags=0) noexcept : ScopeWrapper(ImGui::BeginPopupModal(name, p_open, flags)) {}
	static constexpr void(*dtor)() IMGUI_NOEXCEPT = ImGui::EndPopup;
};

struct TabBar : public ScopeWrapper<TabBar>
{
    TabBar(const char* name, ImGuiTabBarFlags flags=0) noexcept : ScopeWrapper(ImGui::BeginTabBar(name, flags)) {}
    static constexpr void(*dtor)() IMGUI_NOEXCEPT = ImGui::EndTabBar;
};

struct TabItem : public ScopeWrapper<TabItem>
{
    TabItem(const char* name, bool* open=nullptr, ImGuiTabItemFlags flags=0) noexcept : ScopeWrapper(ImGui::BeginTabItem(name, open, flags)) {}
    static constexpr void(*dtor)() IMGUI_NOEXCEPT = ImGui::EndTabItem;
};

}
