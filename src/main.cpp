#include "PCH.h"
#include "SaveManager.h"
#include "PhysicsFreezer.h"
#include "PCBManager.h"
#include "FSMPManager.h"
#include "CBPCManager.h"
#include "ThreadSafetyManager.h"

void InitializeLogging() {
    auto path = logger::log_directory();
    if (!path) {
        return;
    }

    *path /= fmt::format("{}.log", "Savegamefix");
    auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
    auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

    log->set_level(spdlog::level::info);
    log->flush_on(spdlog::level::info);

    spdlog::set_default_logger(std::move(log));
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] [%s:%#] %v");
}

SKSEPluginInfo(
    .Version = { 1, 1, 0, 0 },
    .Name = "Savegamefix",
    .Author = "You",
    .RuntimeCompatibility = SKSE::VersionIndependence::AddressLibrary
);

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
    InitializeLogging();
    SKSE::Init(a_skse);
    SKSE::AllocTrampoline(128);

    SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* a_msg) {
        switch (a_msg->type) {
            case SKSE::MessagingInterface::kDataLoaded:
                logger::info("Data loaded. Registering systems...");
                SaveManager::Install();
                PCBManager::Install();
                FSMPManager::Install();
                CBPCManager::Install();
                ThreadSafetyManager::Install();
                PhysicsFreezer::Install();
                break;
        }
    });

    logger::info("Savegamefix loaded successfully.");
    return true;
}
