#pragma once

// "DEFER" macro "DEFER", for scheduling a function call for the end of the scope.
// E.g.
// DEFER(ImGui::End(););
// if (!ImGui::Begin("window"))
//   return;
//
#define IMH_CONCAT_IMPL(s1, s2) s1##s2
#define IMH_CONCAT(s1, s2)      IMH_CONCAT_IMPL(s1, s2)
#define DEFER(op)                                                                                  \
    const Deferral IMH_CONCAT(__deferral__, __LINE__)                                                    \
    {                                                                                              \
        []() {                                                                                     \
            op                                                                                     \
        }                                                                                          \
    }
class Deferral
{
    void (*mFn)();

public:
    constexpr Deferral(void (*fn)()) noexcept : mFn(fn) {}
    ~Deferral() noexcept { mFn(); }
};
