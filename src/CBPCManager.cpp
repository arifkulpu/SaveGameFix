#include "CBPCManager.h"
#include "SaveManager.h"

void CBPCManager::Install() {
    Hooks::Install();
}

void CBPCManager::Hooks::Install() {
    auto handle = GetModuleHandleA("cbp.dll");
    if (!handle) {
        logger::warn("cbp.dll not found. Skipping CBPC hooks.");
        return;
    }

    logger::info("CBPC Update Hook: Searching for pattern in cbp.dll...");
    // Pattern scanning logic for CBPC::Update would go here
}

void CBPCManager::Hooks::Update(void* a_this, float a_delta) {
    if (SaveManager::IsSaving()) {
        return; // Roadmap Step 3: Suspend CBPC during save
    }
    DefaultUpdate(a_this, a_delta);
}
