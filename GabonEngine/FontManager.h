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
	// ��ʱһ��Font��ʵ��Ӧ�ý��������Font��
	bool Init();
	void SwitchFont(std::string fontName);
	void Print(const char* sentence, Vector2 screenPos, Vector2 size);
	void Render();
private:
	Font* m_CurFont;
	std::map<std::string, Font*> m_Fonts;
};

