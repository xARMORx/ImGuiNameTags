#ifndef _MAIN_H_
#define _MAIN_H_

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <Windows.h>
#include <memory>
#include <kthook/kthook.hpp>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "APatch/Patch.h"
#include "imguiFunctions.hpp"
#include "CRect.h"
#include "CVector.h"
#include "SAMP.hpp"
#include "afk.h"


using CTimerPrototype = void(__cdecl*)();

using CPlayerTagsNameSignature = void(__fastcall*)(void*, void*, CVector*, const char*, unsigned long, float, bool, int);
using CPlayerTagsHealthSitnature = void(__fastcall*)(void*, void*, CVector*, float, float, float);
using CPlayerTagsLostSitnature = void(__fastcall*)(void*, void*);

CVector convertGameCoordsToScreen(CVector worldCoords)
{
    using CalcScreenCoors_t = bool(__cdecl*)(
        CVector*, CVector*, float*, float*);

    CVector screenCoords;

    auto gameFunction{ reinterpret_cast<CalcScreenCoors_t>(0x71DA00U) };

    float unusedParams[2U]{ 0 };
    gameFunction(&worldCoords, &screenCoords,
        &unusedParams[0U], &unusedParams[1U]);

    return screenCoords;
}

uint32_t fromArgb(uint32_t argb)
{
	return
		// Source is in format: 0xAARRGGBB
		((argb & 0x00FF0000) >> 16) | //______RR
		((argb & 0x0000FF00)) | //____GG__
		((argb & 0x000000FF) << 16) | //___BB____
		((argb & 0xFF000000));         //AA______
	// Return value is in format:  0xAABBGGRR 
}

IDirect3DTexture9* tImage = nullptr;

class AsiPlugin{
public:
	kthook::kthook_simple<CTimerPrototype> CTimerHook{ 0x561B10 };
	kthook::kthook_simple<CPlayerTagsNameSignature> CPlayerNameHook{};
	kthook::kthook_simple<CPlayerTagsHealthSitnature> CPlayerHealthHook{};
	kthook::kthook_simple<CPlayerTagsLostSitnature> CPlayerTagsLostHook{};

	void CTimer__Update(const decltype(CTimerHook)&);
	explicit AsiPlugin();
	virtual ~AsiPlugin();
} AsiPlugin;

#endif