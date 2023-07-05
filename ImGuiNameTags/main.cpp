#include "main.h"
#include <regex>

int screenW = GetSystemMetrics(SM_CXSCREEN);
int screenH = GetSystemMetrics(SM_CYSCREEN);

void CPlayerTags__DrawHealthBar(const decltype(AsiPlugin::CPlayerHealthHook)& hook, void* this_, void* EDX, CVector* pPosition, float health, float armor, float dist) {
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImDrawList* dl = ImGui::GetBackgroundDrawList();

	if (SAMP::getDistanceToCamera(pPosition) <= dist * 1.3f) {
		pPosition->z = SAMP::getDistanceToCamera(pPosition) * 0.020499999f + pPosition->z + 0.2f;
		CVector posOnScreen = convertGameCoordsToScreen(*pPosition);
		CRect rect(0, 0, screenW, screenH);
		if (rect.IsPointInside(posOnScreen.x, posOnScreen.y)) {
			if (armor > 0) {
				drawBar(dl, posOnScreen.x - 25.f, posOnScreen.y - 10.f, 50.0f, 5.0f, 0xFFC8C8C8, 0xFF282828, 2, armor, 3.0f);
				drawBar(dl, posOnScreen.x - 25.f, posOnScreen.y, 50.0f, 5.0f, 0xFF2822B9, 0xFF140B4B, 2, health, 3.0f);
			}
			else {
				drawBar(dl, posOnScreen.x - 25.f, posOnScreen.y - 10.f, 50.0f, 5.0f, 0xFF2822B9, 0xFF140B4B, 2, health, 3.0f);
			}
		}
	}


	ImGui::End();
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void CPlayerTags__DrawLabel(const decltype(AsiPlugin::CPlayerNameHook)& hook, void* this_, void* EDX, CVector* pPosition, const char* szText, D3DCOLOR color, float fDistanceToCamera, bool bDrawStatus, int nStatus) {
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImDrawList* dl = ImGui::GetBackgroundDrawList();

	if (SAMP::getDistanceToCamera(pPosition) <= fDistanceToCamera * 1.3f) {
		pPosition->z = SAMP::getDistanceToCamera(pPosition) * 0.020499999f + pPosition->z + 0.2f;
		CVector posOnScreen = convertGameCoordsToScreen(*pPosition);
		std::regex regular("(\\b\\w+\\b).*\\((\\d+)\\)");
		std::smatch match;
		const std::string str = szText;
		if (std::regex_search(str, match, regular)) {
			std::string a = match[1];
			std::string b = match[2];
			float totalSize = ImGui::CalcTextSize(a.c_str()).x / 2.f + 5.f + ImGui::CalcTextSize(b.c_str()).x / 2.f;
			drawShadowText(dl, (posOnScreen.x) - totalSize, posOnScreen.y - (ImGui::CalcTextSize(a.c_str()).y * 1.8f), a, fromArgb(color));
			drawShadowText(dl, (posOnScreen.x) + totalSize - ImGui::CalcTextSize(std::string("[" + b + "]").c_str()).x + 5.f, posOnScreen.y - (ImGui::CalcTextSize(a.c_str()).y * 1.8f), std::string("[" + b + "]"), 0xFFFFFFFF);
			if (bDrawStatus && nStatus == 2) {
				dl->AddImage(tImage, ImVec2(posOnScreen.x - 50.f, posOnScreen.y - 10.f), ImVec2(posOnScreen.x - 30.f, posOnScreen.y + 10.f));
			}
		}
	}

	ImGui::End();
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void CPlayerTags__OnLostDevice(const decltype(AsiPlugin::CPlayerTagsLostHook)& hook, void* this_, void* EDX) {
	ImGui_ImplDX9_InvalidateDeviceObjects();
	hook.get_trampoline()(this_, EDX);
}

void AsiPlugin::CTimer__Update(const decltype(CTimerHook)& hook) {
	static bool init{};

	if (!init && SAMP::IsSAMPInitialized()) {
		CPlayerNameHook.set_dest(SAMP::GetNameTagsAddr());
		CPlayerNameHook.set_cb(&CPlayerTags__DrawLabel);
		CPlayerNameHook.install();

		CPlayerHealthHook.set_dest(SAMP::GetHealthBarAddr());
		CPlayerHealthHook.set_cb(&CPlayerTags__DrawHealthBar);
		CPlayerHealthHook.install();

		CPlayerTagsLostHook.set_dest(SAMP::GetPlayerTagsLost());
		CPlayerTagsLostHook.set_cb(&CPlayerTags__OnLostDevice);
		CPlayerTagsLostHook.install();
		
		SAMP::nopZalupa();

		IDirect3DDevice9* pDevice = *reinterpret_cast<IDirect3DDevice9**>(0xC97C28);

		ImGui::CreateContext();
		ImGui_ImplWin32_Init(**reinterpret_cast<HWND**>(0xC17054));
		ImGui_ImplDX9_Init(pDevice);
#pragma warning(push)
#pragma warning(disable: 4996)
		std::string font{ getenv("WINDIR") }; font += "\\Fonts\\Arialbd.TTF";
#pragma warning(pop)
		ImGui::GetIO().Fonts->Clear();
		ImGui::GetIO().Fonts->AddFontFromFileTTF(font.c_str(), SAMP::GetFontSize() - 2.0f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
		if (tImage == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, &afk, sizeof(afk), 64, 64, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &tImage);

		init = { true };
	}

	hook.get_trampoline()();
}

AsiPlugin::AsiPlugin() {
	CTimerHook.set_cb([this](auto&&... args) { return AsiPlugin::CTimer__Update(args...); });
	CTimerHook.install();
}

AsiPlugin::~AsiPlugin() {
	CPlayerTagsLostHook.remove();
	CPlayerHealthHook.remove();
	CPlayerNameHook.remove();
	CTimerHook.remove();
}