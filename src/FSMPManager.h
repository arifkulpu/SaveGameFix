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

private:
    FSMPManager() = default;
    hdt::PluginInterface* m_fsmpInterface{ nullptr };
};
