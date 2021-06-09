#pragma once

#include "imguiwrap.h"

// Provides a helper, "DEFER", for scheduling a function call for the end of the scope.
// E.g.
// DEFER(ImGui::End(););
// if (!ImGui::Begin("window"))
//   return;
//
struct Deferral
{
    void (*mFn)(void);
    constexpr Deferral(void (*fn)(void)) noexcept : mFn(fn) {}
    ~Deferral() noexcept { mFn(); }
};

#define IMH_CONCAT_IMPL(s1, s2) s1##s2
#define IMH_CONCAT(s1, s2) IMH_CONCAT_IMPL(s1, s2)
#define DEFER(op) Deferral IMH_CONCAT(__deferral__, __LINE__) { [] () { op } }

// ImGui fails to provide these useful constants
constexpr int ImGuiTableFlags_SizingShift = 13;
constexpr int ImGuiTableFlags_SizingBits  = 4;
