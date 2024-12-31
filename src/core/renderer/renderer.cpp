#include "renderer.h"
#include "../hooks/hooks.h"
#include "../utils/console.h"
#include "../../gui/gui.h"
#include "../../features/manager.h"

#include <d3d11.h>
#include <dxgi.h>
#include <cstdio>

#include "../../../ext/imgui/imgui.h"
#include "../../../ext/imgui/imgui_impl_win32.h"
#include "../../../ext/imgui/imgui_impl_dx11.h"

#pragma comment(lib, "d3d11.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include <mutex>

namespace Renderer {
	ID3D11Device* Device = nullptr;
	ID3D11DeviceContext* Context = nullptr;
	ID3D11RenderTargetView* RenderTargetView = nullptr;
	IDXGISwapChain* SwapChain = nullptr;
	HWND Window = nullptr;
	WNDPROC OriginalWndProc = nullptr;
	bool Initialized = false;
	bool ShowMenu = false;

	std::recursive_mutex ImGuiMutex;

	typedef HRESULT(__stdcall* t_Present)(IDXGISwapChain* chain, UINT sync_interval, UINT flags);
	t_Present o_Present = nullptr;

	typedef HRESULT(__stdcall* t_ResizeBuffers)(IDXGISwapChain* chain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swap_chain_flags);
	t_ResizeBuffers o_ResizeBuffers = nullptr;

	void* present_addr = nullptr;
	void* resize_addr = nullptr;

	// hooks
	LRESULT CALLBACK h_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		if (!Initialized)
			return CallWindowProc(OriginalWndProc, hWnd, uMsg, wParam, lParam);

		if (uMsg == WM_KEYDOWN && wParam == VK_INSERT) {
			ShowMenu = !ShowMenu;
		}

		// TODO: fix input blocking, find a better way to handle input
		if (ShowMenu) {
			std::lock_guard<std::recursive_mutex> lock(ImGuiMutex);
			ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
			// Block all input when menu is open
			return 1;
		}

		return CallWindowProc(OriginalWndProc, hWnd, uMsg, wParam, lParam);
	}

	HRESULT __stdcall h_ResizeBuffers(IDXGISwapChain* chain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swap_chain_flags) {
		if (!Device) {
			return o_ResizeBuffers(chain, buffer_count, width, height, new_format, swap_chain_flags);
		}

		if (RenderTargetView) {
			Context->OMSetRenderTargets(0, 0, 0);
			RenderTargetView->Release();
			RenderTargetView = nullptr;
		}

		HRESULT hr = o_ResizeBuffers(chain, buffer_count, width, height, new_format, swap_chain_flags);

		ID3D11Texture2D* buffer = nullptr;
		chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);
		if (buffer) {
			Device->CreateRenderTargetView(buffer, nullptr, &RenderTargetView);
			buffer->Release();
			Context->OMSetRenderTargets(1, &RenderTargetView, nullptr);

			D3D11_VIEWPORT vp;
			vp.Width = (FLOAT)width;
			vp.Height = (FLOAT)height;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			Context->RSSetViewports(1, &vp);
		}

		return hr;
	}

	// it will run every frame
	HRESULT __stdcall h_Present(IDXGISwapChain* chain, UINT sync_interval, UINT flags) {
		if (!Initialized) {
			if (FAILED(chain->GetDevice(__uuidof(ID3D11Device), (void**)&Device))) {
				LOG(LOG_ERROR, "Failed to get device from swapchain");
				return o_Present(chain, sync_interval, flags);
			}

			Device->GetImmediateContext(&Context);

			DXGI_SWAP_CHAIN_DESC desc;
			chain->GetDesc(&desc);
			Window = desc.OutputWindow;

			ID3D11Texture2D* buffer = nullptr;
			chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);
			if (buffer) {
				Device->CreateRenderTargetView(buffer, nullptr, &RenderTargetView);
				buffer->Release();
			}
			else {
				LOG(LOG_ERROR, "Failed to get buffer from swapchain");
				return o_Present(chain, sync_interval, flags);
			}

			// init ImGui
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

			ImGui_ImplWin32_Init(Window);
			ImGui_ImplDX11_Init(Device, Context);

			GUI::ApplyStyle();

			OriginalWndProc = (WNDPROC)SetWindowLongPtr(Window, GWLP_WNDPROC, (LONG_PTR)h_WndProc);

			ShowMenu = true;
			Initialized = true;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		{
			std::lock_guard<std::recursive_mutex> lock(ImGuiMutex);
			ImGui::NewFrame();
		}

		FeatureManager::Update();

		GUI::Render();

		ImGui::Render();
		Context->OMSetRenderTargets(1, &RenderTargetView, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		return o_Present(chain, sync_interval, flags);
	}

	bool Init() {
		// temporary window to get VTable
		WNDCLASSEX wc;
		ZeroMemory(&wc, sizeof(wc));
		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = DefWindowProc;
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpszClassName = "DummyWindow";
		RegisterClassEx(&wc);

		HWND hWnd = CreateWindow(wc.lpszClassName, "Dummy Window", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, wc.hInstance, NULL);

		if (!hWnd) {
			printf("[!] Failed to create dummy window\n");
			return false;
		}

		// dummy device to get VTable
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* context = nullptr;
		IDXGISwapChain* chain = nullptr;
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
			D3D11_SDK_VERSION, &sd, &chain, &device, &featureLevel, &context
		);

		if (FAILED(hr)) {
			LOG(LOG_ERROR, "Failed to create dummy device: %lx", hr);
			DestroyWindow(hWnd);
			UnregisterClass(wc.lpszClassName, wc.hInstance);
			return false;
		}

		void** vtable = *reinterpret_cast<void***>(chain);
		present_addr = vtable[8];
		resize_addr = vtable[13];

		chain->Release();
		device->Release();
		context->Release();

		// cleanup temp window
		DestroyWindow(hWnd);
		UnregisterClass(wc.lpszClassName, wc.hInstance);

		// hook Present
		if (!Hooks::Create(present_addr, &h_Present, (void**)&o_Present)) return false;
		if (!Hooks::Enable(present_addr)) return false;

		// hook ResizeBuffers
		if (!Hooks::Create(resize_addr, &h_ResizeBuffers, (void**)&o_ResizeBuffers)) return false;
		if (!Hooks::Enable(resize_addr)) return false;

		LOG(LOG_INFO, "Hooks for gui initialized!");
		return true;
	}

	void Destroy() {
		// restore WndProc
		if (OriginalWndProc && Window) {
			SetWindowLongPtr(Window, GWLP_WNDPROC, (LONG_PTR)OriginalWndProc);
		}

		// unhook
		if (Initialized) {
			Hooks::Remove(present_addr);
			Hooks::Remove(resize_addr);
		}
	}
}
