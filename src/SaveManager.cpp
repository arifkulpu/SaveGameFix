#include "SaveManager.h"
#include "PhysicsFreezer.h"
#include "PCBManager.h"
#include "ThreadSafetyManager.h"
#include "FSMPManager.h"

void SaveManager::Install() {
    Hooks::Install();
    logger::info("SaveManager hooks installed.");
}

void SaveManager::Hooks::Install() {
    auto& trampoline = SKSE::GetTrampoline();
    
    // Using REL::ID(35711) for BGSSaveLoadManager::Save in 1.6.1170
    REL::Relocation<std::uintptr_t> target{ REL::ID(35711) }; 
    
    logger::info("Hooking BGSSaveLoadManager::Save at address: 0x{:X}", target.address());
    
    // Use write_branch to maintain the hook chain
    _Save = trampoline.write_branch<5>(target.address(), Save);
    
    logger::info("Trampoline allocated at: 0x{:X}", _Save.address());
}

bool SaveManager::Hooks::Save(RE::BGSSaveLoadManager* a_this, const char* a_fileName) {
    if (!a_this) {
        return false;
    }

    OnPreSave();
    
    // Call original function
    // Note: If the engine actually uses 4 arguments, they are still in R8/R9.
    // Our C++ function didn't touch them yet (hopefully).
    bool result = _Save(a_this, a_fileName);
    
    OnPostSave();
    
    return result;
}


void SaveManager::OnPreSave() {
    s_isSaving = true;
    logger::info("Pre-Save: Suspending physics...");
    
    ThreadSafetyManager::PrepareForSave();
    
    // Use the new FSMP control instead of the async console command
    FSMPManager::GetSingleton()->DisablePhysics(true);
    
    PhysicsFreezer::FreezeAll();
    
    logger::info("Pre-Save: Ready.");
}

void SaveManager::OnPostSave() {
    s_isSaving = false;
    logger::info("Post-Save: Resuming systems...");
    
    PhysicsFreezer::UnfreezeAll();
    
    // Resume FSMP
    FSMPManager::GetSingleton()->DisablePhysics(false);
    
    ThreadSafetyManager::FinalizeAfterSave();
    
    logger::info("Post-Save: Done.");
}
