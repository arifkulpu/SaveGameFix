#pragma once

#include "PCH.h"

class PCBManager : public RE::BSTEventSink<RE::TESCellAttachDetachEvent> {
public:
    static void Install();
    static void RequestPCB(bool a_force = false);
    
    virtual RE::BSEventNotifyControl ProcessEvent(const RE::TESCellAttachDetachEvent* a_event, RE::BSTEventSource<RE::TESCellAttachDetachEvent>* a_eventSource) override;

private:
    PCBManager() = default;
    static PCBManager* GetSingleton();

    static inline float s_lastPCBTime = 0.0f;
    static constexpr float PCB_COOLDOWN = 15.0f * 60.0f; // 15 minutes
};
