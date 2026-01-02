#include "dllmain.h"
#include "entry.h"
#include <Windows.h>
#include <thread>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#ifdef NO_LL

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH: {
        std::thread t([]() { bedrock_world_map::init(); });
        t.detach();
        break;
    }

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        bedrock_world_map::disable();
        break;
    }
    return TRUE; // Successful DLL_PROCESS_ATTACH.
}

#else

#include "ll/api/mod/RegisterHelper.h"

namespace bedrock_world_map {

BedrockWorldMapMod& BedrockWorldMapMod::getInstance() {
    static BedrockWorldMapMod instance;
    return instance;
}

bool BedrockWorldMapMod::load() {
    getSelf().getLogger().debug("Loading...");
    bedrock_world_map::init();
    return true;
}

bool BedrockWorldMapMod::enable() {
    getSelf().getLogger().debug("Enabling...");
    // Code for enabling the mod goes here.
    return true;
}

bool BedrockWorldMapMod::disable() {
    getSelf().getLogger().debug("Disabling...");
    // Code for disabling the mod goes here.
    return true;
}

} // namespace bedrock_world_map

LL_REGISTER_MOD(bedrock_world_map::BedrockWorldMapMod, bedrock_world_map::BedrockWorldMapMod::getInstance());
#endif