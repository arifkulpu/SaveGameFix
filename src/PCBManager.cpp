#include "PCBManager.h"
#include <chrono>

void PCBManager::Install() {
    auto scriptEventSource = RE::ScriptEventSourceHolder::GetSingleton();
    if (scriptEventSource) {
        scriptEventSource->AddEventSink(GetSingleton());
        logger::info("PCBManager registered for cell attach events.");
    }
}

PCBManager* PCBManager::GetSingleton() {
    static PCBManager singleton;
    return &singleton;
}

void PCBManager::RequestPCB() {
    auto player = RE::PlayerCharacter::GetSingleton();
    if (!player) return;

    if (player->IsInCombat()) {
        logger::info("Player is in combat, delaying PCB.");
        return;
    }

    auto calendar = RE::Calendar::GetSingleton();
    if (!calendar) return;

    float currentTime = calendar->GetDaysPassed() * 24.0f * 60.0f * 60.0f; // Convert days to in-game seconds, or use real time.
    // For real time cooldown:
    static auto lastTime = std::chrono::steady_clock::now() - std::chrono::seconds(15 * 60 + 1);
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastTime).count();

    if (duration < 15 * 60) {
        logger::info("PCB is on cooldown. Time elapsed: {} seconds.", duration);
        return;
    }

    lastTime = now;
    
    logger::info("Executing PCB command...");
    // Run PCB command
    if (const auto taskQueue = SKSE::GetTaskInterface()) {
        taskQueue->AddTask([]() {
            RE::ConsoleLog::GetSingleton()->Print("Running SafeSave PCB...");
            
            // To run an actual console command safely:
            auto scriptFactory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::Script>();
            if (scriptFactory) {
                RE::Script* script = scriptFactory->Create();
                if (script) {
                    script->SetCommand("pcb");
                    script->CompileAndRun(nullptr);
                    delete script;
                }
            }
        });
    }
}

RE::BSEventNotifyControl PCBManager::ProcessEvent(const RE::TESCellAttachDetachEvent* a_event, RE::BSTEventSource<RE::TESCellAttachDetachEvent>* a_eventSource) {
    if (a_event && a_event->reference && a_event->reference->IsPlayerRef()) {
        logger::info("Player changed cell. Requesting conditional PCB...");
        RequestPCB();
    }
    return RE::BSEventNotifyControl::kContinue;
}
