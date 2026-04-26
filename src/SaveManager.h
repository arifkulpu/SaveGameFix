#pragma once

#include "PCH.h"

class SaveManager {
public:
    static void Install();

private:
    static void OnSaveEvent(SKSE::MessagingInterface::Message* a_msg);
    static void OnPreSave();
    static void OnPostSave();
};
