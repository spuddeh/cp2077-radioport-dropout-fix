// ======================================================================================
// Mod Name: Radioport Dropout Fix
// Author: Spuddeh
// Description: Stops the Radioport going silent while a passing car is tuned to the same station.
// File Version: 1.2.1
// Credits: RED4ext by WopsS.
// ======================================================================================
//
// **The game plays at most four radio stations at once: the four whose nearest receiver is closest
// to the listener.** The radio manager rebuilds that list from every station's receivers, picking
// one receiver per station to measure the station's distance by. The pick is the fault: a receiver
// tagged `pocket_radio_emitter` is used only when NO other receiver in the world is tuned to the
// station, and any other receiver replaces it with no distance test and no check that its radio is
// on. So a traffic car tuned to the player's station, radio off, a hundred metres away, becomes
// that station's measuring point, and wherever four other stations have a receiver closer than the
// car the player's station falls out of the four and every receiver playing it is switched off
// until the car leaves.
//
// The fix is one instruction in the rebuild loop. `mov r15b, al` carries the tag test's answer into
// the choice; `xor r15b, r15b` makes every receiver untagged, so the nearest one represents its
// station and the Radioport, on the player, always wins for the station it is on.
//
// **The function is resolved by RED4ext hash and every byte is verified before any is written.**
// A different game build fails the check and nothing is patched, with a log line saying so.
//
// **A second patch keeps a radio audible at low master volume.** Every player radio sound (the
// Radioport and the car radio) is heard only through a user-defined aux send. The game hands Wwise
// connection thresholds of {-70, -40, -40, -40} dB (direct, game-defined send, user-defined send,
// reflections), and Wwise drops any send below its threshold. At low master volume a radio's send sits
// near -40 dB, so a small cut (its own slider, the scanner, gunfire) silences it completely. The
// user-defined send threshold becomes -96 dB, the voice threshold, before the sound engine starts. The
// array is patched only where exactly one copy of it exists in the exe's read-only data; game 2.31
// sets no connection thresholds and has no copy.

#include <Windows.h>
#include <RED4ext/RED4ext.hpp>

#include <cstdint>
#include <cstring>
#include <string>

namespace
{
// The radio manager's list-rebuild function, 0x9da83c on 2.31.
constexpr uint32_t kHashListRebuild = 905582677;

// Its first bytes, so a resolved address that is not this function is refused.
constexpr uint8_t kPrologue[] = {0x48, 0x8B, 0xC4, 0x55, 0x53, 0x56, 0x57, 0x41};

// The patch site and the instructions around it:
//   +0x147  49 8B 10     mov rdx, [r8]          the receiver's vtable
//   +0x14a  49 8B C8     mov rcx, r8
//   +0x14d  44 8A F8     mov r15b, al           al = "is this receiver the pocket radio"
//   +0x150  FF 52 50     call [rdx+0x50]        the receiver's position
constexpr size_t kSite = 0x147;
constexpr uint8_t kExpected[] = {0x49, 0x8B, 0x10, 0x49, 0x8B, 0xC8, 0x44, 0x8A, 0xF8, 0xFF, 0x52, 0x50};
constexpr size_t kPatchAt = 0x14d;
constexpr uint8_t kPatched[] = {0x45, 0x30, 0xFF};  // xor r15b, r15b

// The connection thresholds the game passes to AK::SoundEngine::SetConnectionVolumeThresholds.
constexpr float kSendThresholds[] = {-70.0f, -40.0f, -40.0f, -40.0f};
constexpr size_t kUserSendIndex = 2;
constexpr float kUserSendThreshold = -96.0f;

const RED4ext::v1::Sdk* g_sdk = nullptr;
RED4ext::v1::PluginHandle g_handle = nullptr;

void Log(const std::string& aText)
{
    if (g_sdk && g_sdk->logger)
    {
        g_sdk->logger->Info(g_handle, aText.c_str());
    }
}

uintptr_t ResolveByHash(uint32_t aHash)
{
    using ResolveFn = uintptr_t (*)(uint32_t);
    const HMODULE red4ext = GetModuleHandleW(L"RED4ext.dll");
    const auto resolve = red4ext ? reinterpret_cast<ResolveFn>(GetProcAddress(red4ext, "RED4ext_ResolveAddress"))
                                 : nullptr;
    return resolve ? resolve(aHash) : 0;
}

bool WriteBytes(void* aAt, const void* aData, size_t aLen)
{
    DWORD old = 0;
    if (!VirtualProtect(aAt, aLen, PAGE_EXECUTE_READWRITE, &old))
    {
        return false;
    }
    std::memcpy(aAt, aData, aLen);
    VirtualProtect(aAt, aLen, old, &old);
    FlushInstructionCache(GetCurrentProcess(), aAt, aLen);
    return true;
}

void Patch()
{
    const auto fn = reinterpret_cast<uint8_t*>(ResolveByHash(kHashListRebuild));
    if (!fn)
    {
        Log("the radio manager's list-rebuild function did not resolve - nothing patched");
        return;
    }
    if (std::memcmp(fn, kPrologue, sizeof(kPrologue)) != 0)
    {
        Log("the resolved function does not start as expected - not this game build, nothing patched");
        return;
    }
    if (std::memcmp(fn + kSite, kExpected, sizeof(kExpected)) == 0)
    {
        if (!WriteBytes(fn + kPatchAt, kPatched, sizeof(kPatched)))
        {
            Log("the patch site could not be made writable - nothing patched");
            return;
        }
        Log("patched: the Radioport now counts like any other receiver when a station is measured");
        return;
    }
    uint8_t already[sizeof(kExpected)];
    std::memcpy(already, kExpected, sizeof(already));
    std::memcpy(already + (kPatchAt - kSite), kPatched, sizeof(kPatched));
    if (std::memcmp(fn + kSite, already, sizeof(already)) == 0)
    {
        Log("the patch is already in place - another copy of this plugin, or a mod carrying the same fix");
        return;
    }
    Log("the patch site does not hold the expected bytes - not this game build, nothing patched");
}
// Every copy of aPattern in the exe's read-only data.
size_t FindInRdata(const void* aPattern, size_t aLen, uint8_t** aFirst)
{
    const auto base = reinterpret_cast<uint8_t*>(GetModuleHandleW(nullptr));
    const auto dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
    const auto nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    auto section = IMAGE_FIRST_SECTION(nt);
    size_t count = 0;
    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++section)
    {
        if (std::memcmp(section->Name, ".rdata", 6) != 0)
        {
            continue;
        }
        uint8_t* start = base + section->VirtualAddress;
        const size_t size = section->Misc.VirtualSize;
        for (size_t at = 0; at + aLen <= size; at += 4)
        {
            if (std::memcmp(start + at, aPattern, aLen) == 0)
            {
                if (count++ == 0)
                {
                    *aFirst = start + at;
                }
            }
        }
    }
    return count;
}

void PatchSendThreshold()
{
    uint8_t* at = nullptr;
    const size_t copies = FindInRdata(kSendThresholds, sizeof(kSendThresholds), &at);
    if (copies != 1)
    {
        float patched[4];
        std::memcpy(patched, kSendThresholds, sizeof(patched));
        patched[kUserSendIndex] = kUserSendThreshold;
        uint8_t* already = nullptr;
        if (copies == 0 && FindInRdata(patched, sizeof(patched), &already) == 1)
        {
            Log("the send threshold is already -96 dB - another copy of this plugin, or a mod carrying the same fix");
            return;
        }
        Log(copies == 0 ? "no connection thresholds in this game build - send threshold left alone"
                        : "the connection thresholds appear more than once - send threshold left alone");
        return;
    }
    if (!WriteBytes(at + kUserSendIndex * sizeof(float), &kUserSendThreshold, sizeof(kUserSendThreshold)))
    {
        Log("the connection thresholds could not be made writable - send threshold left alone");
        return;
    }
    Log("patched: a radio's send is kept down to -96 dB, so it stays audible at low master volume");
}
} // namespace

RED4EXT_C_EXPORT void RED4EXT_CALL Query(RED4ext::v1::PluginInfo* aInfo)
{
    aInfo->name = L"RadioportDropoutFix";
    aInfo->author = L"Spuddeh";
    aInfo->version = RED4EXT_V1_SEMVER(1, 2, 1);
    aInfo->runtime = RED4EXT_V1_RUNTIME_VERSION_INDEPENDENT;
    aInfo->sdk = RED4EXT_V1_SDK_VERSION_CURRENT;
}

RED4EXT_C_EXPORT uint32_t RED4EXT_CALL Supports()
{
    return RED4EXT_API_VERSION_1;
}

RED4EXT_C_EXPORT bool RED4EXT_CALL Main(RED4ext::v1::PluginHandle aHandle,
                                        RED4ext::v1::EMainReason aReason, const RED4ext::v1::Sdk* aSdk)
{
    if (aReason == RED4ext::v1::EMainReason::Load)
    {
        g_sdk = aSdk;
        g_handle = aHandle;
        Patch();
        PatchSendThreshold();
    }
    return true;
}
