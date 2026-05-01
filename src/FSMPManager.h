#pragma once

#include "PCH.h"
#include "PluginAPI.h"

class FSMPManager : public hdt::IPreStepListener {
public:
    static FSMPManager* GetSingleton();
    static void Install();

    // From SKSE messaging
    void OnFSMPMessage(SKSE::MessagingInterface::Message* a_msg);

    // Physics control
    void LockSimulation();
    void UnlockSimulation();
    void DisablePhysics(bool a_disable);

    // IPreStepListener
    virtual RE::BSEventNotifyControl ProcessEvent(const hdt::PreStepEvent* a_event, RE::BSTEventSource<hdt::PreStepEvent>* a_eventSource) override;

    struct Hooks {
        static void Update(void* a_this, float a_delta);
        static inline REL::Relocation<decltype(Update)> DefaultUpdate;

        static void Install();
    };

private:
    FSMPManager() = default;
    hdt::PluginInterface* m_fsmpInterface{ nullptr };

    void* m_skyrimPhysicsWorld{ nullptr };
    uint32_t m_disabledOffset{ 0 };
    
    // Member function pointers
    using GetWorld_t = void* (*)();
    using LockSimulation_t = void* (*)(void*, void*); // returns unique_lock by value (complex)

    GetWorld_t f_GetWorld{ nullptr };
    LockSimulation_t f_LockSimulation{ nullptr };
    
    void InitializeInternal();
};
