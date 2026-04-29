# Savegamefix (Safe Save & Physics Guard for Skyrim)

This SKSE plugin acts as a **"Traffic Police"** for your game engine, specifically designed to eliminate "Exception Access Violation" crashes during the save process in heavily modded setups (especially those using FSMP and CBPC).

## Technical Roadmap & Features

### 1. Project Infrastructure
- **CommonLibSSE-NG**: Fully compatible with Skyrim AE (1.6.1170) and other versions.
- **Address Library**: Uses version-independent offsets to ensure stability across updates.

### 2. Save Hooking (Main Hook)
- Hooks `RE::BGSSaveLoadManager::Save` directly.
- Manages a global `isSaving` state to coordinate all engine systems.

### 3. Physics Guard (FSMP & CBPC)
- **Direct Hooks**: Hooks the update loops of `hdtSMP64.dll` and `cbp.dll`.
- **Zero-Latency Suspension**: If `isSaving` is active, physics updates return immediately, preventing the physics engine from accessing memory while the game is serializing it.

### 4. Thread & Memory Safety
- **ScrapHeap Monitoring**: Checks memory status before allowing the save to proceed.
- **TaskQueue Management**: Minimizes background task interference during the critical save window.

### 5. Quicksave Reliability
- Refactors Quicksaves to be processed as Manual Saves internally, preventing data corruption common in rapid save/load cycles.

### 6. Post-Save Cleanup
- **Automatic Resume**: Re-enables physics loops immediately after the file is written.
- **Forced PCB**: Executes `Purge Cell Buffers` to clear temporary memory and prevent late-save bloat.

### 7. Diagnostics
- Detailed logging to `Documents/My Games/Skyrim Special Edition/SKSE/Savegamefix.log`.
- Console feedback for system status.

## Requirements
- **Skyrim Anniversary Edition (1.6.1170)**
- **SKSE64** (v2.2.6+)
- **Address Library for SKSE Plugins**
- **Faster HDT-SMP (FSMP)** and/or **CBPC**

---
"The best answer to 'it's unnecessary' is a crash-free session."

# Savegamefix (Skyrim için Güvenli Kayıt ve Fizik Koruması)

Bu SKSE eklentisi, oyun motorunuz için bir **"Trafik Polisi"** görevi görerek, özellikle ağır modlanmış sistemlerde (FSMP ve CBPC kullanan) kayıt işlemi sırasında oluşan "Exception Access Violation" çökmelerini ortadan kaldırmak için tasarlanmıştır.

## Teknik Yol Haritası ve Özellikler

### 1. Proje Altyapısı
- **CommonLibSSE-NG**: Skyrim AE (1.6.1170) ve diğer tüm sürümlerle tam uyumluluk.
- **Address Library**: Güncellemelerde modun bozulmaması için sürümden bağımsız ofsetler kullanımı.

### 2. Kayıt Yakalama (Main Hook)
- `RE::BGSSaveLoadManager::Save` fonksiyonunu doğrudan kancalar.
- Tüm sistemleri koordine etmek için global bir `isSaving` durumu yönetir.

### 3. Fizik Koruması (FSMP & CBPC)
- **Doğrudan Kancalar**: `hdtSMP64.dll` ve `cbp.dll` dosyalarının güncelleme döngülerini kancalar.
- **Sıfır Gecikmeli Duraklatma**: Eğer `isSaving` aktifse, fizik güncellemeleri anında durur (return), böylece oyun verileri yazılırken fizik motorunun belleğe erişmesi engellenir.

### 4. İş Parçacığı ve Bellek Güvenliği
- **ScrapHeap İzleme**: Kayıt başlamadan önce bellek durumunu kontrol eder.
- **Görev Kuyruğu Yönetimi**: Kayıt penceresinde arka plan görevlerinin (Tasklet) müdahalesini en aza indirir.

### 5. Hızlı Kayıt (Quicksave) Güvenilirliği
- Hızlı Kayıtları (F5) dahili olarak "Manuel Kayıt" gibi işleyerek veri bozulmalarını önler.

### 6. Kayıt Sonrası Temizlik
- **Otomatik Devam**: Dosya yazıldıktan hemen sonra fizik döngülerini tekrar aktif eder.
- **Zorunlu PCB**: Bellek şişmesini önlemek için `Purge Cell Buffers` komutunu zorunlu olarak çalıştırır.

### 7. Teşhis ve Loglama
- `Documents/My Games/Skyrim Special Edition/SKSE/Savegamefix.log` dosyasına detaylı günlük kaydı.
- Sistem durumu hakkında konsol geri bildirimi.

## Gereksinimler
- **Skyrim Anniversary Edition (1.6.1170)**
- **SKSE64** (v2.2.6+)
- **Address Library for SKSE Plugins**
- **Faster HDT-SMP (FSMP)** ve/veveya **CBPC**

---
"Gereksiz diyenlere en güzel cevap, çökme yaşanmayan bir oyun seansıdır."
