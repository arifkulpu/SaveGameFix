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

void PCBManager::RequestPCB(bool a_force) {
    auto player = RE::PlayerCharacter::GetSingleton();
    if (!player) return;

    if (!a_force && player->IsInCombat()) {
        logger::info("Player is in combat, delaying PCB.");
        return;
    }

    static auto lastTime = std::chrono::steady_clock::now() - std::chrono::seconds(15 * 60 + 1);
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastTime).count();

    if (!a_force && duration < 15 * 60) {
        logger::info("PCB is on cooldown. Time elapsed: {} seconds.", duration);
        return;
    }

    lastTime = now;
    
    logger::info("Executing PCB command...");
    // Run PCB command
    if (const auto taskQueue = SKSE::GetTaskInterface()) {
        taskQueue->AddTask([]() {
            auto* consoleLog = RE::ConsoleLog::GetSingleton();
            if (consoleLog) {
                consoleLog->Print("Running SafeSave PCB...");
            }

            auto* player = RE::PlayerCharacter::GetSingleton();
            if (!player) {
                logger::warn("PCBManager::RequestPCB: PlayerCharacter NULL, PCB atlandı.");
                return;
            }

            auto scriptFactory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::Script>();
            if (!scriptFactory) {
                logger::error("PCBManager::RequestPCB: Script factory NULL!");
                return;
            }

            RE::Script* script = scriptFactory->Create();
            if (!script) {
                logger::error("PCBManager::RequestPCB: Script oluşturulamadı!");
                return;
            }

            script->SetCommand("pcb");
            // Player ref’ı geçirmek CompileAndRun’ın context'siz çalışmasını önler
            script->CompileAndRun(player);
            logger::info("PCB komutu başarıyla çalıştırıldı.");
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
