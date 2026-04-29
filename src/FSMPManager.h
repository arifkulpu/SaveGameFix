#pragma once

#include "PCH.h"
#include "PluginAPI.h"

class FSMPManager : public hdt::IPreStepListener {
public:
    static FSMPManager* GetSingleton();
    static void Install();

    // From SKSE messaging
    void OnFSMPMessage(SKSE::MessagingInterface::Message* a_msg);

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
};
