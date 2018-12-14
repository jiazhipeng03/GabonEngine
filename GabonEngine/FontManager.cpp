#include "FontManager.h"
#include "d3dUtil.h"
#include "MainApp.h"
FontManager::FontManager()
	: m_CurFont(nullptr)
{

}


FontManager::~FontManager()
{
	for (auto f : m_Fonts)
	{
		SafeDelete(f.second);
	}
	m_Fonts.clear();
	m_CurFont = nullptr;
}

bool FontManager::Init()
{
	Font* defaultFont = new Font;
	TextureShader* shader = g_App->GetShaderMan()->GetShader("bitmap");
	bool bRet = defaultFont->Init("Font/fontdata.txt", /*"Font/font.dds"*/"Textures/font.dds", shader);
	m_Fonts["default"] = defaultFont;
	m_CurFont = defaultFont;
	return bRet;
}

void FontManager::SwitchFont(std::string fontName)
{
	auto miter = m_Fonts.find(fontName);
	if (miter != m_Fonts.end())
	{
		m_CurFont = miter->second;
	}
}

void FontManager::Print(const char* sentence, Vector2 screenPos, Vector2 size)
{	
	m_CurFont->ReleaseVertices();
	m_CurFont->UpdateVertices(sentence, screenPos, size);	
}

void FontManager::Render()
{
	m_CurFont->Render();
	//m_CurFont->ReleaseBuffer();
}
