#pragma once

#include <vector>
#include <optional>
#include <string_view>
#include <Windows.h>
#include <Psapi.h>

namespace Scanner {
    inline std::optional<uintptr_t> FindPattern(HMODULE a_module, std::string_view a_pattern) {
        if (!a_module) return std::nullopt;

        MODULEINFO mi;
        if (!GetModuleInformation(GetCurrentProcess(), a_module, &mi, sizeof(mi))) return std::nullopt;

        auto base = reinterpret_cast<const uint8_t*>(mi.lpBaseOfDll);
        auto size = mi.SizeOfImage;

        auto parsePattern = [](std::string_view pattern) {
            std::vector<std::optional<uint8_t>> bytes;
            for (size_t i = 0; i < pattern.size(); ++i) {
                if (pattern[i] == ' ') continue;
                if (pattern[i] == '?') {
                    bytes.push_back(std::nullopt);
                    if (i + 1 < pattern.size() && pattern[i + 1] == '?') i++;
                } else {
                    bytes.push_back(static_cast<uint8_t>(std::strtol(&pattern[i], nullptr, 16)));
                    i++;
                }
            }
            return bytes;
        };

        auto patternBytes = parsePattern(a_pattern);
        for (size_t i = 0; i < size - patternBytes.size(); ++i) {
            bool found = true;
            for (size_t j = 0; j < patternBytes.size(); ++j) {
                if (patternBytes[j] && base[i + j] != *patternBytes[j]) {
                    found = false;
                    break;
                }
            }
            if (found) return reinterpret_cast<uintptr_t>(&base[i]);
        }

        return std::nullopt;
    }

    inline std::optional<uintptr_t> FindString(HMODULE a_module, std::string_view a_string) {
        if (!a_module) return std::nullopt;

        MODULEINFO mi;
        if (!GetModuleInformation(GetCurrentProcess(), a_module, &mi, sizeof(mi))) return std::nullopt;

        auto base = reinterpret_cast<const uint8_t*>(mi.lpBaseOfDll);
        auto size = mi.SizeOfImage;

        for (size_t i = 0; i < size - a_string.size(); ++i) {
            if (memcmp(&base[i], a_string.data(), a_string.size()) == 0) {
                return reinterpret_cast<uintptr_t>(&base[i]);
            }
        }

        return std::nullopt;
    }

    inline std::optional<uintptr_t> FindXRef(HMODULE a_module, uintptr_t a_address) {
        if (!a_module) return std::nullopt;

        MODULEINFO mi;
        if (!GetModuleInformation(GetCurrentProcess(), a_module, &mi, sizeof(mi))) return std::nullopt;

        auto base = reinterpret_cast<const uint8_t*>(mi.lpBaseOfDll);
        auto size = mi.SizeOfImage;

        for (size_t i = 0; i < size - 4; ++i) {
            // Check for LEA or MOV with RIP-relative addressing
            // Usually 48 8D or 4C 8D or 48 8B ...
            // We search for the 32-bit offset that points to a_address
            int32_t offset = *reinterpret_cast<const int32_t*>(&base[i]);
            if (reinterpret_cast<uintptr_t>(&base[i + 4]) + offset == a_address) {
                // Verify if it's a common instruction prefix
                uint8_t prefix = base[i - 3];
                uint8_t op = base[i - 2];
                uint8_t modrm = base[i - 1];
                
                // This is a bit simplified, but should work for common cases
                if (op == 0x8D || op == 0x8B) return reinterpret_cast<uintptr_t>(&base[i - 3]);
            }
        }

        return std::nullopt;
    }
}
