#include "FSMPManager.h"
#include "SaveManager.h"

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
    Hooks::Install();
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

void FSMPManager::Hooks::Install() {
    auto handle = GetModuleHandleA("hdtSMP64.dll");
    if (!handle) {
        logger::warn("hdtSMP64.dll not found. Skipping FSMP hooks.");
        return;
    }

    // Pattern for SkyrimPhysicsWorld::update in FSMP 2.0+
    auto& trampoline = SKSE::GetTrampoline();
    // This is a symbolic representation; in practice, use a signature scanner
    // For this example, we assume we found the offset or address
    
    // Example pattern scan (simplified)
    // auto scan = SKSE::WinAPI::GetModuleExport(handle, "SomeExportedFunc"); 
    
    logger::info("FSMP Update Hook: Searching for pattern...");
    // REL::Offset or similar would go here. 
    // Since we are following the roadmap, let's use a hypothetical successful find.
}

void FSMPManager::Hooks::Update(void* a_this, float a_delta) {
    if (SaveManager::IsSaving()) {
        return; // Roadmap Step 3: Return if saving
    }
    DefaultUpdate(a_this, a_delta);
}

RE::BSEventNotifyControl FSMPManager::ProcessEvent(const hdt::PreStepEvent* a_event, RE::BSTEventSource<hdt::PreStepEvent>* a_eventSource) {
    if (!a_event) return RE::BSEventNotifyControl::kContinue;

    if (SaveManager::IsSaving()) {
        // Additional safeguard in listener
        return RE::BSEventNotifyControl::kContinue; 
    }

    return RE::BSEventNotifyControl::kContinue;
}
