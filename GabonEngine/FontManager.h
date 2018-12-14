#pragma once

#include <map>
#include <string>
#include "Font.h"
class Font;
class FontManager
{
public:
	FontManager();
	~FontManager();
	// 暂时一个Font，实际应该接入第三方Font库
	bool Init();
	void SwitchFont(std::string fontName);
	void Print(const char* sentence, Vector2 screenPos, Vector2 size);
	void Render();
private:
	Font* m_CurFont;
	std::map<std::string, Font*> m_Fonts;
};

