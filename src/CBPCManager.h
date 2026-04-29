#pragma once

#include "PCH.h"

class CBPCManager {
public:
    static void Install();

    struct Hooks {
        static void Update(void* a_this, float a_delta);
        static inline REL::Relocation<decltype(Update)> DefaultUpdate;

        static void Install();
    };
};
