#pragma once

#include <utility>  // std::forward etc

#include "imgui.h"

#ifndef DEAR_NO_STRING
#    include <string>
#endif

#include "imguiwrap.helpers.h"

namespace dear
{
    static const ImVec2 Zero(0.0f, 0.0f);

    // EditTableFlags provides a window with checkboxes/selects for all of the
    // ImGuiTableFlags options so that a flags property can be edited in real-time.
    extern void
    EditTableFlags(const char* editWindowTitle, bool* showing, ImGuiTableFlags* flags) noexcept;

    // EditWindowFlags presents a window for selecting text input field flags.
    extern void
    EditInputTextFlags(const char* title, bool* showing, ImGuiInputTextFlags* flags) noexcept;

    // EditWindowFlags provides a window which checkboxes for all of the
    // ImGuiWindowFlags options so that a flags property can be edited in real-time.
    extern void
    EditWindowFlags(const char* editWindowTitle, bool* showing, ImGuiWindowFlags* flags) noexcept;

    // SetHostWindowSize lets you alter the native window dimensions from within your `imgui_main`
    // callback. Change is applied after control returns to the imgui_main function, and the
    // sizing from the last call within a frame is used.
    extern void SetHostWindowSize(int x, int y) noexcept;

    // scoped_effect is a helper that uses automatic object lifetime to control
    // the invocation of a callable after potentially calling additional code,
    // allowing for easy inline creation of scope guards.
    //
    // On its own, it does nothing but call the supplied function when it is
    // destroyed;
    template<typename Base, bool ForceDtor = false>
    struct ScopeWrapper
    {
        using wrapped_type = Base;
        using self_type    = ScopeWrapper<Base>;

        static constexpr bool force_dtor = ForceDtor;

    protected:
        const bool ok_;

    public:
        // constructor takes a predicate that may be used to determine if
        // additional calls can be made, and a function/lambda/callable to
        // be invoked from the destructor.
        constexpr ScopeWrapper(bool ok) noexcept : ok_{ok} {}

        // destructor always invokes the supplied destructor function.
        ~ScopeWrapper() noexcept
        {
            if constexpr (!force_dtor) {
                if (!ok_)
                    return;
            }
            Base::dtor();
        }

        // operator&& will excute 'code' if the predicate supplied during
        // construction was true.
        template<typename PassthruFn>
        constexpr bool operator&&(PassthruFn passthru) const noexcept
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

    // Wrapper for ImGui::Begin ... End, which will always call End.
    struct Begin : public ScopeWrapper<Begin, true>
    {
        // Invoke Begin and guarantee that 'End' will be called.
        Begin(const char* title, bool* open = nullptr, ImGuiWindowFlags flags = 0) noexcept
            : ScopeWrapper(ImGui::Begin(title, open, flags))
        {}
        static void dtor() noexcept { ImGui::End(); }
    };

    // Wrapper for ImGui::BeginChild ... EndChild, which will always call EndChild.
    struct Child : public ScopeWrapper<Child, true>
    {
        Child(const char* title, const ImVec2& size = Zero, ImGuiChildFlags child_flags = 0,
              ImGuiWindowFlags flags = 0) noexcept
            : ScopeWrapper(ImGui::BeginChild(title, size, child_flags, flags))
        {}
        Child(ImGuiID id, const ImVec2& size = Zero, ImGuiChildFlags child_flags = 0,
              ImGuiWindowFlags flags = 0) noexcept
            : ScopeWrapper(ImGui::BeginChild(id, size, child_flags, flags))
        {}
        static void dtor() noexcept { ImGui::EndChild(); }
    };

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    // Wrapper for ImGui::BeginChildFrame ... EndChildFrame, which will always call EndChildFrame.
    struct ChildFrame : public ScopeWrapper<ChildFrame, true>
    {
        template<typename... Args>
        ChildFrame(Args&&... args) noexcept
            : ScopeWrapper(ImGui::BeginChildFrame(std::forward<Args>(args)...))
        {}
        static void dtor() noexcept { ImGui::EndChildFrame(); }
    };
#endif

    // Wrapper for ImGui::BeginGroup ... EndGroup which will always call EndGroup.
    struct Group : public ScopeWrapper<Group, true>
    {
        Group() noexcept : ScopeWrapper(true) { ImGui::BeginGroup(); }
        static void dtor() noexcept { ImGui::EndGroup(); }
    };

    // Wrapper for ImGui::Begin...EndCombo.
    struct Combo : public ScopeWrapper<Combo>
    {
        Combo(const char* label, const char* preview, ImGuiComboFlags flags = 0) noexcept
            : ScopeWrapper(ImGui::BeginCombo(label, preview, flags))
        {}
        static void dtor() noexcept { ImGui::EndCombo(); }
    };

    // Wrapper for ImGui::Begin...EndListBox.
    struct ListBox : public ScopeWrapper<ListBox>
    {
        ListBox(const char* label, const ImVec2& size = Zero) noexcept
            : ScopeWrapper(ImGui::BeginListBox(label, size))
        {}
        static void dtor() noexcept { ImGui::EndListBox(); }
    };

    // Wrapper for ImGui::Begin...EndMenuBar.
    struct MenuBar : public ScopeWrapper<MenuBar>
    {
        MenuBar() noexcept : ScopeWrapper(ImGui::BeginMenuBar()) {}
        static void dtor() noexcept { ImGui::EndMenuBar(); }
    };

    // Wrapper for ImGui::Begin...EndMainMenuBar.
    struct MainMenuBar : public ScopeWrapper<MainMenuBar>
    {
        MainMenuBar() noexcept : ScopeWrapper(ImGui::BeginMainMenuBar()) {}
        static void dtor() noexcept { ImGui::EndMainMenuBar(); }
    };

    // Wrapper for ImGui::BeginMenu...ImGui::EndMenu.
    struct Menu : public ScopeWrapper<Menu>
    {
        Menu(const char* label, bool enabled = true) noexcept
            : ScopeWrapper(ImGui::BeginMenu(label, enabled))
        {}
        static void dtor() noexcept { ImGui::EndMenu(); }
    };

    // Wrapper for ImGui::BeginTable...ImGui::EndTable.
    // See also EditTableFlags.
    struct Table : public ScopeWrapper<Table>
    {
        Table(const char* str_id, int column, ImGuiTableFlags flags = 0,
              const ImVec2& outer_size = Zero, float inner_width = 0.0f) noexcept
            : ScopeWrapper(ImGui::BeginTable(str_id, column, flags, outer_size, inner_width))
        {}
        static void dtor() noexcept { ImGui::EndTable(); }
    };

    // Wrapper for ImGui::Begin...EndToolTip.
    struct Tooltip : public ScopeWrapper<Tooltip>
    {
        Tooltip(bool enabled = true) noexcept : ScopeWrapper(enabled && ImGui::BeginTooltip()) {  }
        static void dtor() noexcept { ImGui::EndTooltip(); }
    };

    // Wrapper around ImGui::CollapsingHeader to allow consistent code styling.
    struct CollapsingHeader : public ScopeWrapper<CollapsingHeader>
    {
        CollapsingHeader(const char* label, ImGuiTreeNodeFlags flags = 0) noexcept
            : ScopeWrapper(ImGui::CollapsingHeader(label, flags))
        {}
        inline static void dtor() noexcept {}
    };

    // Wrapper for ImGui::TreeNode...ImGui::TreePop.
    // See also SeparatedTreeNode.
    struct TreeNode : public ScopeWrapper<TreeNode>
    {
        template<typename... Args>
        TreeNode(Args&&... args) noexcept
            : ScopeWrapper(ImGui::TreeNode(std::forward<Args>(args)...))
        {}
        static void dtor() noexcept { ImGui::TreePop(); }
    };

    // Wrapper around a TreeNode followed by a Separator (it's a fairly common sequence).
    struct SeparatedTreeNode : public ScopeWrapper<SeparatedTreeNode>
    {
        template<typename... Args>
        SeparatedTreeNode(Args&&... args) noexcept
            : ScopeWrapper(ImGui::TreeNode(std::forward<Args>(args)...))
        {}
        static void dtor() noexcept
        {
            ImGui::TreePop();
            ImGui::Separator();
        }
    };

	// Wrapper for ImGui::TreeNode...ImGui::TreePop.
	// See also SeparatedTreeNode.
	struct TreeNodeEx : public ScopeWrapper<TreeNodeEx>
	{
		template<typename... Args>
		TreeNodeEx(Args&&... args) noexcept
			: ScopeWrapper(ImGui::TreeNodeEx(std::forward<Args>(args)...))
		{}
		static void dtor() noexcept { ImGui::TreePop(); }
	};

	// Wrapper around a TreeNode followed by a Separator (it's a fairly common sequence).
	struct SeparatedTreeNodeEx : public ScopeWrapper<SeparatedTreeNodeEx>
	{
		template<typename... Args>
		SeparatedTreeNodeEx(Args&&... args) noexcept
			: ScopeWrapper(ImGui::TreeNodeEx(std::forward<Args>(args)...))
		{}
		static void dtor() noexcept
		{
			ImGui::TreePop();
			ImGui::Separator();
		}
	};

    // Popup provides the stock wrapper around ImGui::BeginPopup...ImGui::EndPopup as well as two
    // methods of instantiating a modal, for those who want modality to be a property fo Popup
    // rather than a discrete type.
    struct Popup : public ScopeWrapper<Popup>
    {
        // Non-modal Popup.
        Popup(const char* str_id, ImGuiWindowFlags flags = 0) noexcept
            : ScopeWrapper(ImGui::BeginPopup(str_id, flags))
        {}

        // Modal popups.

        // imguiwrap provides 3 ways to construct a modal popup:
        // - Use the PopupModal class,
        // - Use Popup(modal{}, ...)
        // - Use the static method Popup::Modal(...)

        struct modal
        {
        };
        Popup(modal, const char* name, bool* p_open = nullptr, ImGuiWindowFlags flags = 0) noexcept
            : ScopeWrapper(ImGui::BeginPopupModal(name, p_open, flags))
        {}

        static Popup
        Modal(const char* name, bool* p_open = nullptr, ImGuiWindowFlags flags = 0) noexcept
        {
            return Popup(modal{}, name, p_open, flags);
        }

        static void dtor() noexcept { ImGui::EndPopup(); }
    };

    // Wrapper around ImGui's BeginPopupModal ... EndPopup sequence.
    struct PopupModal : public ScopeWrapper<PopupModal>
    {
        PopupModal(const char* name, bool* p_open = nullptr, ImGuiWindowFlags flags = 0) noexcept
            : ScopeWrapper(ImGui::BeginPopupModal(name, p_open, flags))
        {}
        static void dtor() noexcept { ImGui::EndPopup(); }
    };

    // Wrapper for ImGui::BeginTabBar ... EndTabBar
    struct TabBar : public ScopeWrapper<TabBar>
    {
        TabBar(const char* name, ImGuiTabBarFlags flags = 0) noexcept
            : ScopeWrapper(ImGui::BeginTabBar(name, flags))
        {}
        static void dtor() noexcept { ImGui::EndTabBar(); }
    };

    // Wrapper for ImGui::BeginTabItem ... EndTabItem
    struct TabItem : public ScopeWrapper<TabItem>
    {
        TabItem(const char* name, bool* open = nullptr, ImGuiTabItemFlags flags = 0) noexcept
            : ScopeWrapper(ImGui::BeginTabItem(name, open, flags))
        {}
        static void dtor() noexcept { ImGui::EndTabItem(); }
    };

    // Wrapper around pushing a style var onto ImGui's stack and popping it back off.
    /// TODO: Support nesting so we can do a single pop operation.
    struct WithStyleVar : public ScopeWrapper<WithStyleVar>
    {
        WithStyleVar(ImGuiStyleVar idx, const ImVec2& val) noexcept : ScopeWrapper(true)
        {
            ImGui::PushStyleVar(idx, val);
        }
        WithStyleVar(ImGuiStyleVar idx, float val = 0.0f) noexcept : ScopeWrapper(true)
        {
            ImGui::PushStyleVar(idx, val);
        }
        static void dtor() noexcept { ImGui::PopStyleVar(); }
    };

    /// TODO: WithStyleColor

    // Wrapper for BeginTooltip predicated on the previous item being hovered.
    struct ItemTooltip : public ScopeWrapper<ItemTooltip>
    {
        ItemTooltip(ImGuiHoveredFlags flags = ImGuiHoveredFlags_ForTooltip) noexcept
            : ScopeWrapper(ImGui::IsItemHovered(flags))
        {
            if (ok_)
                ImGui::BeginTooltip();
        }
        static void dtor() noexcept { ImGui::EndTooltip(); }
    };

	struct WithID : public ScopeWrapper<WithID>
	{
		template<typename... Args>
		WithID(Args&&... args) noexcept : ScopeWrapper(true)
		{
			ImGui::PushID(std::forward<Args>(args)...);
		}
		static void dtor() noexcept
		{
			ImGui::PopID();
		}
	};

	struct Disabled : public ScopeWrapper<Disabled>
	{

		Disabled(bool disabled) noexcept : ScopeWrapper(true)
		{
			ImGui::BeginDisabled(disabled); // Ideally we would only call this if disabled is true, but because dtor is static, we can't keep track of state
		}
		static void dtor() noexcept
		{
			ImGui::EndDisabled();
		}
	};



//// Text helpers

// std::string helpers.
#ifndef DEAR_NO_STRING
    static inline void Text(const std::string& str) noexcept
    {
        ImGui::TextUnformatted(str.c_str(), str.c_str() + str.length());
    }
    inline void TextUnformatted(const std::string& str) noexcept
    {
        ImGui::TextUnformatted(str.c_str(), str.c_str() + str.length());
    }
#endif

    static inline bool
    MenuItem(const char* text, bool selected = false, bool enabled = true) noexcept
    {
        return ImGui::MenuItem(text, nullptr, selected, enabled);
    }
    static inline bool MenuItem(const char* text, bool* selected, bool enabled = true) noexcept
    {
        return ImGui::MenuItem(text, nullptr, selected, enabled);
    }
#ifndef DEAR_NO_STRING
    static inline bool MenuItem(const std::string& str, const char* shortcut = nullptr,
                                bool selected = false, bool enabled = true) noexcept
    {
        return ImGui::MenuItem(str.c_str(), shortcut, selected, enabled);
    }
    static inline bool
    MenuItem(const std::string& text, bool* selected, bool enabled = true) noexcept
    {
        return ImGui::MenuItem(text.c_str(), nullptr, selected, enabled);
    }
#endif

    // static inline bool Selectable(const char *text) noexcept { return imgui::Selectable(text); }
    static inline bool
    Selectable(const char* label, bool selected = false, ImGuiSelectableFlags flags = 0,
               const ImVec2& size = Zero) noexcept
    {
        return ImGui::Selectable(label, selected, flags, size);
    }
    static inline bool
    Selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags = 0,
               const ImVec2& size = Zero) noexcept
    {
        return ImGui::Selectable(label, p_selected, flags, size);
    }
#ifndef DEAR_NO_STRING
    static inline bool
    Selectable(const std::string& label, bool selected = false, ImGuiSelectableFlags flags = 0,
               const ImVec2& size = Zero) noexcept
    {
        return ImGui::Selectable(label.c_str(), selected, flags, size);
    }
    static inline bool
    Selectable(const std::string& label, bool* p_selected, ImGuiSelectableFlags flags = 0,
               const ImVec2& size = Zero) noexcept
    {
        return ImGui::Selectable(label.c_str(), p_selected, flags, size);
    }
#endif

}  // namespace dear
