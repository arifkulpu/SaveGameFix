#include "SaveManager.h"
#include "PhysicsFreezer.h"
#include "PCBManager.h"
#include "ThreadSafetyManager.h"
void SaveManager::Install() {
    Hooks::Install();
    logger::info("SaveManager hooks installed.");
}

#include "minhook/MinHook.h"

void SaveManager::Hooks::Install() {
    // Initialize MinHook
    if (MH_Initialize() != MH_OK) {
        logger::critical("Failed to initialize MinHook!");
        return;
    }

    // Using REL::RelocationID(34818, 35727) for Save_Impl (4 arguments in SE, might be 6 in AE, using 6 to be safe)
    REL::Relocation<std::uintptr_t> target{ REL::RelocationID(34818, 35727) }; 
    
    if (target.address()) {
        if (MH_CreateHook((void*)target.address(), (void*)&Save, (void**)&_Save) != MH_OK) {
            logger::critical("Failed to create hook for BGSSaveLoadManager::Save!");
            return;
        }
        if (MH_EnableHook((void*)target.address()) != MH_OK) {
            logger::critical("Failed to enable hook for BGSSaveLoadManager::Save!");
            return;
        }
        logger::info("Hooked BGSSaveLoadManager::Save (Save_Impl) using MinHook at address: 0x{:X}", target.address());
    } else {
        logger::critical("Failed to find BGSSaveLoadManager::Save address!");
    }
}

bool SaveManager::Hooks::Save(RE::BGSSaveLoadManager* a_this, int32_t a_deviceID, uint32_t a_outputStats, const char* a_fileName, bool a_isAutoSave, void* a_unk) {
    if (!a_this) {
        return false;
    }

    OnPreSave();
    
    // Call original function with all 6 arguments
    bool result = _Save(a_this, a_deviceID, a_outputStats, a_fileName, a_isAutoSave, a_unk);
    
    OnPostSave();
    
    return result;
}

void SaveManager::OnPreSave() {
    s_isSaving = true;
    logger::info("Starting Pre-Save routines for Skyrim 1.6.1170...");
    
    ThreadSafetyManager::PrepareForSave();
    PhysicsFreezer::FreezeAll();
    
    logger::info("Pre-Save routines complete.");
}

void SaveManager::OnPostSave() {
    if (!RE::PlayerCharacter::GetSingleton()) {
        s_isSaving = false;
        return;
    }

    s_isSaving = false;
    logger::info("Post-Save: Resuming systems...");
    
    PhysicsFreezer::UnfreezeAll();
    
    ThreadSafetyManager::FinalizeAfterSave();
    
    logger::info("Post-Save: Done.");
}
