#include <Windows.h>
#include <cstdio>
#include "core/utils/console.h"
#include "core/hooks/hooks.h"
#include "core/renderer/renderer.h"

DWORD WINAPI MainThread(LPVOID lpParam) {
	Console::Allocate();
	LOG(LOG_INFO, "DLL Injected!");

	if (!Hooks::Init()) {
		LOG(LOG_ERROR, "Failed to init MinHook");
		FreeLibraryAndExitThread((HMODULE)lpParam, 1);
		return 0;
	}

	if (!Renderer::Init()) {
		LOG(LOG_ERROR, "Failed to init Renderer");
		Hooks::Shutdown();
		FreeLibraryAndExitThread((HMODULE)lpParam, 1);
		return 0;
	}

	LOG(LOG_INFO, "Renderer initialized. Press END to unload.");

	while (!GetAsyncKeyState(VK_END)) {
		Sleep(100);
	}

	LOG(LOG_INFO, "Unloading...");
	Renderer::Destroy();
	Hooks::Shutdown();
	Console::Free();

	FreeLibraryAndExitThread((HMODULE)lpParam, 0);
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
