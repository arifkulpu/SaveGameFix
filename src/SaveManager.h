#pragma once

#include "PCH.h"

class SaveManager {
public:
    static void Install();
    static bool IsSaving() { return s_isSaving; }

private:
    static inline bool s_isSaving = false;

    struct Hooks {
        static bool Save(RE::BGSSaveLoadManager* a_this, const char* a_fileName);
        static inline REL::Relocation<decltype(Save)> _Save;

        static void Install();
    };

    static void OnPreSave();
    static void OnPostSave();
};
