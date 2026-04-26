#include "FSMPManager.h"

FSMPManager* FSMPManager::GetSingleton() {
    static FSMPManager singleton;
    return &singleton;
}

void FSMPManager::Install() {
    auto messaging = SKSE::GetMessagingInterface();
    if (messaging) {
        // FSMP broadcasts messages under its own plugin name "hdtSMP64"
        messaging->RegisterListener("hdtSMP64", [](SKSE::MessagingInterface::Message* a_msg) {
            GetSingleton()->OnFSMPMessage(a_msg);
        });
        logger::info("Registered listener for hdtSMP64.");
    }
}

void FSMPManager::OnFSMPMessage(SKSE::MessagingInterface::Message* a_msg) {
    if (a_msg && a_msg->type == hdt::PluginInterface::MSG_STARTUP && a_msg->data) {
        logger::info("Received MSG_STARTUP from hdtSMP64.");
        m_fsmpInterface = reinterpret_cast<hdt::PluginInterface*>(a_msg->data);
        
        auto info = m_fsmpInterface->getVersionInfo();
        logger::info("FSMP Interface Version: {}.{}.{}", info.interfaceVersion.major, info.interfaceVersion.minor, info.interfaceVersion.patch);
        logger::info("FSMP Bullet Version: {}.{}.{}", info.bulletVersion.major, info.bulletVersion.minor, info.bulletVersion.patch);

        m_fsmpInterface->addListener(this);
        logger::info("Successfully registered FSMP PreStepListener.");
    }
}

RE::BSEventNotifyControl FSMPManager::ProcessEvent(const hdt::PreStepEvent* a_event, RE::BSTEventSource<hdt::PreStepEvent>* a_eventSource) {
    if (!a_event) return RE::BSEventNotifyControl::kContinue;

    // Here you can iterate over a_event->objects and manipulate Bullet physics objects
    // for example, to freeze dead NPCs' physics or suspend operations before save.
    
    // auto& objects = a_event->objects;
    // for (int i = 0; i < objects.size(); ++i) {
    //     btCollisionObject* obj = objects[i];
    //     // manipulate obj
    // }

    return RE::BSEventNotifyControl::kContinue;
}
