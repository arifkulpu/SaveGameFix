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
    auto processLists = RE::ProcessLists::GetSingleton();
    if (!processLists) return;

    for (auto& handle : processLists->highActorHandles) {
        auto actor = handle.get();
        if (actor && actor->IsDead()) {
            // actor->PauseCurrentAnimation(true);
        }
    }
}
