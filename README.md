# Savegamefix (Safe Save & FPS Protector for Skyrim AE)

This SKSE plugin provides two critical features to stabilize your game and improve performance, specifically when dealing with the Faster HDT-SMP physics engine.

## Features

1. **Safe Save Mechanic (Crash Preventer)**
   - When you trigger a save (Quick Save, Auto Save, or Manual Save), the plugin automatically sends a silent `smp off` command to instantly suspend all physics calculations.
   - After the save process is completely written to the disk, it sends an `smp on` command to resume physics and executes the `pcb` (Purge Cell Buffers) command to clear memory safely.
   - This eliminates the infamous "Race Condition" crashes caused by the physics engine trying to update actors while the game is serializing them.

2. **Dead NPC Physics Freezer (FPS Protector)**
   - The plugin actively listens for the `TESDeathEvent`.
   - The moment an NPC dies, their underlying Havok animation state is permanently frozen. 
   - Because Faster HDT-SMP relies on the base skeleton's movement to calculate physics, freezing the animation effectively puts their SMP physics (hair, cloaks, armor) to sleep.
   - This prevents massive FPS drops during and after large battles with many corpses.

## Requirements
- **Skyrim Anniversary Edition (1.6.1170)**
- **SKSE64** (matching your game version)
- **Address Library for SKSE Plugins**
- **Faster HDT-SMP (FSMP)**

---

# Savegamefix (Skyrim AE için Güvenli Kayıt ve FPS Koruyucu)

Bu SKSE eklentisi, özellikle Faster HDT-SMP fizik motoruyla oynarken oyununuzu dengelemek ve performansı artırmak için iki kritik özellik sunar.

## Özellikler

1. **Güvenli Kayıt Mekaniği (Safe Save)**
   - Kayıt işlemi başlattığınızda (Hızlı Kayıt, Otomatik Kayıt veya Normal Kayıt), eklenti arka planda sessizce `smp off` komutunu göndererek dünyadaki tüm fizik hesaplamalarını anında durdurur.
   - Kayıt işlemi diske güvenle yazıldıktan sonra `smp on` komutuyla fizikleri tekrar başlatır ve hemen ardından `pcb` (Purge Cell Buffers) komutunu çalıştırarak belleği temizler.
   - Bu sistem, oyun motoru NPC'leri kayıt dosyasına yazmaya çalışırken fizik motorunun arka planda hareket hesaplamaya çalışmasından kaynaklanan meşhur çökme (CTD) sorunlarını %100 ortadan kaldırır.

2. **Ölü NPC Fizik Dondurucu (FPS Koruyucu)**
   - Eklenti, arka planda ölüm olaylarını (`TESDeathEvent`) dinler.
   - Bir NPC öldüğü an, temel Havok animasyon iskeleti tamamen dondurulur.
   - Faster HDT-SMP, saç ve pelerinleri hareket ettirmek için alt iskeletin ivmesine ihtiyaç duyduğundan, iskeletin donması SMP fiziklerinin de otomatik olarak uyku moduna geçmesini sağlar.
   - Bu sayede, çok sayıda cesedin bulunduğu büyük savaşlardan sonra yaşanan devasa FPS düşüşlerinin önüne geçilir.

## Gereksinimler
- **Skyrim Anniversary Edition (1.6.1170)**
- **SKSE64** (Oyun sürümünüzle uyumlu)
- **Address Library for SKSE Plugins**
- **Faster HDT-SMP (FSMP)**
