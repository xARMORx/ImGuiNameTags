#ifndef _MAIN_H_
#define _MAIN_H_

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <Windows.h>
#include <memory>
#include "memwrapper/memwrapper.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "imguiFunctions.hpp"
#include "SAMP.hpp"
#include "sampapi/CPlayerPool.h"
#include "sampapi/CNetGame.h"
#include "sampapi/CVector.h"
#include "sampapi/CRect.h"
#include "afk.h"


using GameloopPrototype = void(__cdecl*)();
std::unique_ptr<memwrapper::memhook<GameloopPrototype>> GameloopHook;

using PresentSignature = HRESULT(__stdcall*)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
std::unique_ptr<memwrapper::memhook<PresentSignature>> PresentHook;

using ResetSignature = HRESULT(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
std::unique_ptr<memwrapper::memhook<ResetSignature>> ResetHook;

sampapi::CVector convertGameCoordsToScreen(sampapi::CVector worldCoords)
{
    using CalcScreenCoors_t = bool(__cdecl*)(
        sampapi::CVector*, sampapi::CVector*, float*, float*);

    sampapi::CVector screenCoords;

    auto gameFunction{ reinterpret_cast<CalcScreenCoors_t>(0x71DA00U) };

    float unusedParams[2U]{ 0 };
    gameFunction(&worldCoords, &screenCoords,
        &unusedParams[0U], &unusedParams[1U]);

    return screenCoords;
}

bool isPointOnScreen(sampapi::CRect* rect, sampapi::CVector* pos) {
    return ((bool(__thiscall*)(sampapi::CRect*, sampapi::CVector*))(0x404290))(rect, pos);
}

class AsiPlugin{
public:
	explicit AsiPlugin();
	virtual ~AsiPlugin();
} AsiPlugin;

#endif