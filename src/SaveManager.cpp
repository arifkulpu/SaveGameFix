#include "SaveManager.h"
#include "PhysicsFreezer.h"
#include "PCBManager.h"
#include "ThreadSafetyManager.h"
void SaveManager::Install() {
    Hooks::Install();
    logger::info("SaveManager hooks installed.");
}

#include "minhook/MinHook.h"

void SaveManager::Hooks::Install() {
    // Initialize MinHook
    if (MH_Initialize() != MH_OK) {
        logger::critical("Failed to initialize MinHook!");
        return;
    }

    // SE ID: 34818, AE ID: 35728 (1.6.1170 için doğrulanmış)
    // Not: AE için Address Library v2 (ae) kullanılıyor.
    REL::Relocation<std::uintptr_t> target{ REL::RelocationID(34818, 35728) };

    const auto addr = target.address();
    if (!addr) {
        logger::critical("Failed to find BGSSaveLoadManager::Save address! Address Library güncel mi?");
        return;
    }

    logger::info("BGSSaveLoadManager::Save adresi bulundu: 0x{:X}", addr);

    if (MH_CreateHook(reinterpret_cast<void*>(addr), reinterpret_cast<void*>(&Save),
                      reinterpret_cast<void**>(&_Save)) != MH_OK) {
        logger::critical("Failed to create hook for BGSSaveLoadManager::Save!");
        return;
    }
    if (MH_EnableHook(reinterpret_cast<void*>(addr)) != MH_OK) {
        logger::critical("Failed to enable hook for BGSSaveLoadManager::Save!");
        return;
    }
    logger::info("Hooked BGSSaveLoadManager::Save (Save_Impl) using MinHook at address: 0x{:X}", addr);
}

bool SaveManager::Hooks::Save(RE::BGSSaveLoadManager* a_this, int32_t a_deviceID, uint32_t a_outputStats,
                               const char* a_fileName, bool a_isAutoSave, void* a_unk) {
    // NULL guard — geçersiz save manager crash'ini önler
    if (!a_this) {
        logger::error("Save hook: BGSSaveLoadManager pointer NULL! Kayıt atlanıyor.");
        return false;
    }

    // Orijinal fonksiyon pointer'ı da validate et
    if (!_Save) {
        logger::error("Save hook: Orijinal _Save pointer NULL! Hook kurulumu başarısız.");
        return false;
    }

    OnPreSave();

    // Orijinal fonksiyonu çağır
    bool result = _Save(a_this, a_deviceID, a_outputStats, a_fileName, a_isAutoSave, a_unk);

    OnPostSave();

    return result;
}

void SaveManager::OnPreSave() {
    s_isSaving = true;
    logger::info("Pre-Save rutinleri başlatılıyor (Skyrim 1.6.1170)...");

    // PlayerCharacter hazır değilse kayıt rutinlerini atla — erken save crash'ini önler
    auto* player = RE::PlayerCharacter::GetSingleton();
    if (!player) {
        logger::warn("OnPreSave: PlayerCharacter NULL — alt sistemler atlanıyor.");
        return;
    }

    // BGSSaveLoadManager bütünlük kontrolü
    auto* slm = RE::BGSSaveLoadManager::GetSingleton();
    if (!slm) {
        logger::warn("OnPreSave: BGSSaveLoadManager NULL — alt sistemler atlanıyor.");
        return;
    }

    // SkyrimSoulsRE gibi no-pause modlarıyla çakışmayı azaltmak için
    // UI menüleri açıkken save yapmaktan kaçın
    auto* ui = RE::UI::GetSingleton();
    if (ui && ui->GameIsPaused()) {
        logger::info("OnPreSave: Oyun duraklatılmış, normal akışta devam ediliyor.");
    } else {
        logger::warn("OnPreSave: Oyun duraklatılmamış (SkyrimSoulsRE aktif olabilir). "
                     "Veri tutarsızlığı riski var.");
    }

    ThreadSafetyManager::PrepareForSave();
    PhysicsFreezer::FreezeAll();

    logger::info("Pre-Save rutinleri tamamlandı.");
}

void SaveManager::OnPostSave() {
    if (!RE::PlayerCharacter::GetSingleton()) {
        s_isSaving = false;
        return;
    }

    s_isSaving = false;
    logger::info("Post-Save: Resuming systems...");
    
    PhysicsFreezer::UnfreezeAll();
    
    ThreadSafetyManager::FinalizeAfterSave();
    
    logger::info("Post-Save: Done.");
}
