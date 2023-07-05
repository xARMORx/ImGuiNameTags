

void drawBar(ImDrawList* dl, float x, float y, float sizeX, float sizeY, unsigned long color, unsigned long bgColor, unsigned char borderSize, float value, float rounding) {
	dl->AddRectFilled(ImVec2(x - borderSize, y - borderSize), ImVec2(x + sizeX + borderSize, y + sizeY + borderSize), 0xFF000000, rounding);
	dl->AddRectFilled(ImVec2(x, y), ImVec2(x + sizeX, y + sizeY), bgColor, rounding);
	dl->AddRectFilled(ImVec2(x, y), ImVec2(x + (sizeX / 100) * value, y + sizeY), color, rounding);
}

void changeFontSize(ImGuiIO io, std::string szFont, float size) {
	std::string font{ getenv("WINDIR") }; font += "\\Fonts\\" + szFont;
	io.Fonts->Clear();
	io.Fonts->AddFontFromFileTTF(font.c_str(), size - 2.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
}

void drawShadowText(ImDrawList* dl, float x, float y, std::string text, unsigned long color) {
	dl->AddText(ImVec2(x + 1.0f, y), 0xFF000000, text.c_str());
	dl->AddText(ImVec2(x, y + 1.0f), 0xFF000000, text.c_str());
	dl->AddText(ImVec2(x - 1.0f, y), 0xFF000000, text.c_str());
	dl->AddText(ImVec2(x, y - 1.0f), 0xFF000000, text.c_str());
	dl->AddText(ImVec2(x, y), color, text.c_str());
}