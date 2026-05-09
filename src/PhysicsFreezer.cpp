#include "PhysicsFreezer.h"
#include "PCH.h"

PhysicsFreezer* PhysicsFreezer::GetSingleton() {
    static PhysicsFreezer singleton;
    return &singleton;
}

void PhysicsFreezer::Install() {
    if (auto scriptEventSource = RE::ScriptEventSourceHolder::GetSingleton()) {
        scriptEventSource->AddEventSink(GetSingleton());
        logger::info("PhysicsFreezer registered for TESDeathEvent.");
    }
}

RE::BSEventNotifyControl PhysicsFreezer::ProcessEvent(const RE::TESDeathEvent* a_event, RE::BSTEventSource<RE::TESDeathEvent>* a_eventSource) {
    if (a_event && a_event->actorDying) {
        if (auto actor = a_event->actorDying->As<RE::Actor>()) {
            logger::info("Actor {} died. Freezing physics.", actor->GetName());
            
            // Wait a few seconds for the death animation/ragdoll to settle, then freeze
            SKSE::GetTaskInterface()->AddTask([actorHandle = actor->GetHandle()]() {
                if (auto a = actorHandle.get()) {
                    // a->PauseCurrentAnimation(true);
                    if (auto root = a->Get3D()) {
                        // root->UpdateKinematic();
                    }
                }
            });
        }
    }
    return RE::BSEventNotifyControl::kContinue;
}

void PhysicsFreezer::FreezeAll() {
    logger::info("Freezing all physics...");
    FreezeDeadNPCs();
}

void PhysicsFreezer::UnfreezeAll() {
    logger::info("Unfreezing all physics...");
}

void PhysicsFreezer::FreezeDeadNPCs() {
    // This function is currently a placeholder for Roadmap Step 3.
    // Removed iteration to ensure thread safety during background saves.
}
