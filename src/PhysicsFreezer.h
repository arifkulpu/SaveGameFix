#include "PCH.h"

class PhysicsFreezer : public RE::BSTEventSink<RE::TESDeathEvent> {
public:
    static PhysicsFreezer* GetSingleton();
    static void Install();

    static void FreezeAll();
    static void UnfreezeAll();
    static void FreezeDeadNPCs();

    virtual RE::BSEventNotifyControl ProcessEvent(const RE::TESDeathEvent* a_event, RE::BSTEventSource<RE::TESDeathEvent>* a_eventSource) override;
};
