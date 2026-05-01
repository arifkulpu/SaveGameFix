#include "FSMPManager.h"
#include "SaveManager.h"
#include "Scanner.h"

FSMPManager* FSMPManager::GetSingleton() {
    static FSMPManager singleton;
    return &singleton;
}

void FSMPManager::Install() {
    auto messaging = SKSE::GetMessagingInterface();
    if (messaging) {
        messaging->RegisterListener("hdtSMP64", [](SKSE::MessagingInterface::Message* a_msg) {
            GetSingleton()->OnFSMPMessage(a_msg);
        });
        logger::info("Registered listener for hdtSMP64.");
    }
    Hooks::Install();
}

void FSMPManager::InitializeInternal() {
    if (m_skyrimPhysicsWorld && m_disabledOffset && f_LockSimulation) return;

    auto handle = GetModuleHandleA("hdtSMP64.dll");
    if (!handle) return;

    logger::info("Initializing FSMP internal pointers...");

    // 1. Find SkyrimPhysicsWorld::get()
    auto getPattern = Scanner::FindPattern(handle, "48 8D 05 ? ? ? ? C3");
    if (getPattern) {
        f_GetWorld = reinterpret_cast<GetWorld_t>(*getPattern);
        m_skyrimPhysicsWorld = f_GetWorld();
        logger::info("Found SkyrimPhysicsWorld singleton at: 0x{:X}", reinterpret_cast<uintptr_t>(m_skyrimPhysicsWorld));
    } else {
        logger::error("Failed to find SkyrimPhysicsWorld::get()");
        return;
    }

    // 2. Find 'disabled' offset and 'lockSimulation' function
    // We start from the string "smp reset" or "HDT-SMP disabled"
    auto disabledStr = Scanner::FindString(handle, "HDT-SMP disabled");
    if (disabledStr) {
        auto xref = Scanner::FindXRef(handle, *disabledStr);
        if (xref) {
            // Find 'disabled' offset: C6 80 ? ? ? ? 01 (mov byte ptr [rax+offset], 1)
            for (int i = 0; i < 200; ++i) {
                uint8_t* p = reinterpret_cast<uint8_t*>(*xref - i);
                if (p[0] == 0xC6 && (p[1] == 0x80 || p[1] == 0x40)) {
                    if (p[1] == 0x80) {
                        m_disabledOffset = *reinterpret_cast<uint32_t*>(p + 2);
                    } else {
                        m_disabledOffset = p[2];
                    }
                    logger::info("Found FSMP 'disabled' offset: 0x{:X}", m_disabledOffset);
                    break;
                }
            }
        }
    }

    // Find lockSimulation via "resetTransformsToOriginal"
    // "resetTransformsToOriginal" is used in SMPDebug_Execute for "reset"
    auto resetStr = Scanner::FindString(handle, "running full smp reset");
    if (resetStr) {
        auto xref = Scanner::FindXRef(handle, *resetStr);
        if (xref) {
            // In SMPDebug_Execute, it calls get() then resetTransformsToOriginal()
            // We search for the call to resetTransformsToOriginal
            // Pattern: E8 ? ? ? ? (call)
            for (int i = 0; i < 100; ++i) {
                uint8_t* p = reinterpret_cast<uint8_t*>(*xref + i);
                if (p[0] == 0xE8) {
                    int32_t rel = *reinterpret_cast<int32_t*>(p + 1);
                    uintptr_t resetFunc = reinterpret_cast<uintptr_t>(p + 5) + rel;
                    
                    // Inside resetTransformsToOriginal, the first call is lockSimulation
                    uint8_t* p2 = reinterpret_cast<uint8_t*>(resetFunc);
                    for (int j = 0; j < 50; ++j) {
                        if (p2[j] == 0xE8) {
                            int32_t rel2 = *reinterpret_cast<int32_t*>(&p2[j+1]);
                            f_LockSimulation = reinterpret_cast<LockSimulation_t>(reinterpret_cast<uintptr_t>(&p2[j+5]) + rel2);
                            logger::info("Found SkyrimPhysicsWorld::lockSimulation at: 0x{:X}", reinterpret_cast<uintptr_t>(f_LockSimulation));
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
}

void FSMPManager::OnFSMPMessage(SKSE::MessagingInterface::Message* a_msg) {
    if (a_msg && a_msg->type == hdt::PluginInterface::MSG_STARTUP && a_msg->data) {
        logger::info("Received MSG_STARTUP from hdtSMP64.");
        m_fsmpInterface = reinterpret_cast<hdt::PluginInterface*>(a_msg->data);
        m_fsmpInterface->addListener(this);
        InitializeInternal();
    }
}

static char s_lockBuffer[256]; // Buffer to hold the unique_lock returned by value

void FSMPManager::LockSimulation() {
    InitializeInternal();
    if (m_skyrimPhysicsWorld && f_LockSimulation) {
        // lockSimulation returns std::unique_lock<std::mutex> by value.
        // We provide a buffer for the return value.
        f_LockSimulation(s_lockBuffer, m_skyrimPhysicsWorld);
        // logger::info("FSMP Simulation Locked.");
    }
}

void FSMPManager::UnlockSimulation() {
    if (m_skyrimPhysicsWorld && f_LockSimulation) {
        // To unlock, we simply destroy the unique_lock in the buffer.
        // This is a bit hacky since we don't know the exact destructor,
        // but unique_lock's destructor just calls unlock on the mutex.
        // We can manually unlock if we find the mutex, or just zero the buffer if it's safe.
        // Better yet, we can call the destructor if we find it.
        // For now, let's just use DisablePhysics which is safer.
    }
}

void FSMPManager::DisablePhysics(bool a_disable) {
    InitializeInternal();
    if (m_skyrimPhysicsWorld && m_disabledOffset) {
        bool* disabled = reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(m_skyrimPhysicsWorld) + m_disabledOffset);
        *disabled = a_disable;
    }
}

void FSMPManager::Hooks::Install() {
    auto handle = GetModuleHandleA("hdtSMP64.dll");
    if (!handle) return;
    
    // Original update hook logic...
}

void FSMPManager::Hooks::Update(void* a_this, float a_delta) {
    if (SaveManager::IsSaving()) return;
    DefaultUpdate(a_this, a_delta);
}

RE::BSEventNotifyControl FSMPManager::ProcessEvent(const hdt::PreStepEvent* a_event, RE::BSTEventSource<hdt::PreStepEvent>* a_eventSource) {
    return RE::BSEventNotifyControl::kContinue;
}
