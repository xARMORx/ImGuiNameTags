#include "main.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

IDirect3DTexture9* tImage = nullptr;

bool enable = true;

namespace samp = sampapi::v037r3;

samp::CPlayerPool* pool;

int screenW = GetSystemMetrics(SM_CXSCREEN);
int screenH = GetSystemMetrics(SM_CYSCREEN);

uint32_t fromArgb(uint32_t argb)
{
	return
		// Source is in format: 0xAARRGGBB
		((argb & 0x00FF0000) >> 16) | //______RR
		((argb & 0x0000FF00)) | //____GG__
		((argb & 0x000000FF) << 16)  | //___BB____
		((argb & 0xFF000000));         //AA______
	// Return value is in format:  0xAABBGGRR 
}

HRESULT __stdcall Hooked_Present(IDirect3DDevice9* pDevice, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion) {
	static bool ImGui_inited = false;
	if (SAMP::IsSAMPInitialized()) {
		if (!ImGui_inited) {
			ImGui::CreateContext();
			ImGui_ImplWin32_Init(**reinterpret_cast<HWND**>(0xC17054));
			ImGui_ImplDX9_Init(pDevice);
#pragma warning(push)
#pragma warning(disable: 4996)
			std::string font{ getenv("WINDIR") }; font += "\\Fonts\\Arialbd.TTF";
#pragma warning(pop)
			ImGui::GetIO().Fonts->AddFontFromFileTTF(font.c_str(), SAMP::GetFontSize() - 2.0f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
			if (tImage == nullptr)
				D3DXCreateTextureFromFileInMemoryEx(pDevice, &afk, sizeof(afk), 64, 64, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &tImage);
			
			SAMP::NopNameTags();

			pool = samp::RefNetGame()->GetPlayerPool();

			ImGui_inited = true;
		}
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImDrawList* dl = ImGui::GetBackgroundDrawList();

		for (int i{}; i < pool->MAX_PLAYERS; i++) {
			samp::CPlayerInfo* playerInfo = pool->m_pObject[i];
			char name[128];
			if (playerInfo != nullptr && !playerInfo->m_bIsNPC) {
				samp::CPed* ped = playerInfo->m_pPlayer->m_pPed;
				if (ped != nullptr ) {
					sampapi::CVector bonePos;
					ped->GetBonePosition(8, &bonePos);
					bonePos.z = ped->GetDistanceToCamera() * 0.030499999 + bonePos.z + 0.2;
					if (ped->GetDistanceToPoint(bonePos) <= 2.5f) {
						sampapi::CVector posOnScreen = convertGameCoordsToScreen(bonePos);
						sampapi::CRect rect{0, 0, screenW, screenH};
						if (rect.IsPointInside(posOnScreen.x, posOnScreen.y)) {
							sprintf(name, "%s [%d]", playerInfo->m_szNick.c_str(), i);
							drawBar(dl, posOnScreen.x - 25.f, posOnScreen.y, 50.0f, 5.0f, 0xFF2822B9, 0xFF140B4B, 1, playerInfo->m_pPlayer->m_fReportedHealth, 3.0f);
							if (playerInfo->m_pPlayer->m_fReportedArmour > 0) {
								drawBar(dl, posOnScreen.x - 25.f, posOnScreen.y - 10.f, 50.0f, 5.0f, 0xFFC8C8C8, 0xFF282828, 1, playerInfo->m_pPlayer->m_fReportedArmour, 3.0f);
								drawShadowText(dl, (posOnScreen.x) - ImGui::CalcTextSize(name).x * 0.5f, posOnScreen.y - (ImGui::CalcTextSize(name).y * 1.8), name, fromArgb(playerInfo->m_pPlayer->GetColorAsARGB()));
							}
							else {
								drawShadowText(dl, (posOnScreen.x) - ImGui::CalcTextSize(name).x * 0.5f, posOnScreen.y - (ImGui::CalcTextSize(name).y * 1.2), name, fromArgb(playerInfo->m_pPlayer->GetColorAsARGB()));
							}
							if (samp::RefNetGame()->m_bNametagStatus && playerInfo->m_pPlayer->GetStatus() == 2) {
								dl->AddImage(tImage, ImVec2(posOnScreen.x - 50.f, posOnScreen.y - 5.f), ImVec2(posOnScreen.x - 30.f, posOnScreen.y + 15.f));
							}
						}
					}
				}
			}
			else
				continue;
		}


		ImGui::End();
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}
	
	return PresentHook->call<IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*>(pDevice, pSrcRect, pDestRect, hDestWindow, pDirtyRegion);
}

HRESULT __stdcall Hooked_Reset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentParams) {

	ImGui_ImplDX9_InvalidateDeviceObjects();
	return ResetHook->call(pDevice, pPresentParams);
}

void InstallD3DHook() {
	DWORD pDevice = *reinterpret_cast<DWORD*>(0xC97C28);
	void** vTable = *reinterpret_cast<void***>(pDevice);

	PresentHook = std::make_unique<memwrapper::memhook<PresentSignature>>(vTable[17], &Hooked_Present);
	PresentHook->install();
	ResetHook = std::make_unique<memwrapper::memhook<ResetSignature>>(vTable[16], &Hooked_Reset);
	ResetHook->install();
}


void GameloopHooked() {
	static bool once = false;
	if (once == false) {
		InstallD3DHook();
		once = true;
	}

	GameloopHook->call();
}

AsiPlugin::AsiPlugin() {
	GameloopHook = std::make_unique<memwrapper::memhook<GameloopPrototype>>(0x00561B10, &GameloopHooked);
	GameloopHook->install();
}



AsiPlugin::~AsiPlugin() {
	ResetHook->remove();
	PresentHook->remove();
	GameloopHook->remove();
}