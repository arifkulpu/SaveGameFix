#include "SaveManager.h"
#include "PhysicsFreezer.h"
#include "PCBManager.h"

void SaveManager::Install() {
    SKSE::GetMessagingInterface()->RegisterListener("SKSE", OnSaveEvent);
    logger::info("SaveManager installed.");
}

void SaveManager::OnSaveEvent(SKSE::MessagingInterface::Message* a_msg) {
    if (a_msg->type == SKSE::MessagingInterface::kSaveGame) {
        OnPreSave();
        
        // Register a task to run on the next frame to re-enable physics
        SKSE::GetTaskInterface()->AddTask([]() {
            OnPostSave();
        });
    }
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
    logger::info("Pre-Save event triggered. Freezing physics.");
    // Freezing Havok and calling smp off
    PhysicsFreezer::FreezeAll();
    ExecuteSMPCommand("smp off");
}

void SaveManager::OnPostSave() {
    logger::info("Post-Save event triggered. Unfreezing physics and scheduling PCB.");
    PhysicsFreezer::UnfreezeAll();
    ExecuteSMPCommand("smp on");
    PCBManager::RequestPCB();
}
