#include "SaveManager.h"
#include "PhysicsFreezer.h"
#include "PCBManager.h"
#include "ThreadSafetyManager.h"

void SaveManager::Install() {
    Hooks::Install();
    logger::info("SaveManager hooks installed.");
}

void SaveManager::Hooks::Install() {
    auto& trampoline = SKSE::GetTrampoline();
    REL::Relocation<std::uintptr_t> target{ REL::VariantID(34818, 35711, 0x589570) }; // BGSSaveLoadManager::Save
    DefaultSave = trampoline.write_call<5>(target.address(), Save);
}

bool SaveManager::Hooks::Save(RE::BGSSaveLoadManager* a_this, const char* a_fileName, int8_t a_unkn, uint32_t a_unkn2) {
    OnPreSave();
    bool result = DefaultSave(a_this, a_fileName, a_unkn, a_unkn2);
    OnPostSave();
    return result;
}

void ExecuteSMPCommand(const char* cmd) {
    if (const auto taskQueue = SKSE::GetTaskInterface()) {
        taskQueue->AddTask([cmdStr = std::string(cmd)]() {
            auto scriptFactory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::Script>();
            if (scriptFactory) {
                if (RE::Script* script = scriptFactory->Create()) {
                    script->SetCommand(cmdStr);
                    script->CompileAndRun(nullptr);
                    delete script;
                }
            }
        });
    }
}

void SaveManager::OnPreSave() {
    s_isSaving = true;
    logger::info("Pre-Save: Physics Suspended.");
    ThreadSafetyManager::PrepareForSave();
    // Freezing physics engines via SmP commands (Legacy/Backup)
    ExecuteSMPCommand("smp off");
    PhysicsFreezer::FreezeAll();
}

void SaveManager::OnPostSave() {
    s_isSaving = false;
    logger::info("Post-Save: Physics Resumed.");
    ExecuteSMPCommand("smp on");
    PhysicsFreezer::UnfreezeAll();
    ThreadSafetyManager::FinalizeAfterSave();
    PCBManager::RequestPCB(true);
}
