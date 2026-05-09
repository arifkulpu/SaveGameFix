#pragma once

#include "PCH.h"

class SaveManager {
public:
    static void Install();
    static bool IsSaving() { return s_isSaving; }

private:
    static inline bool s_isSaving = false;

    struct Hooks {
        static bool Save(RE::BGSSaveLoadManager* a_this, int32_t a_deviceID, uint32_t a_outputStats, const char* a_fileName, bool a_isAutoSave, void* a_unk);
        using Save_t = decltype(&Save);
        static inline Save_t _Save;

        static void Install();
    };

    static void OnPreSave();
    static void OnPostSave();
};
