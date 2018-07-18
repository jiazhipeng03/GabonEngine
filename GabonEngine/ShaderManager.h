#pragma once
#include <map>
#include "TextureShader.h"
class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();
	TextureShader* GetShader(std::string name) { return m_mapShader[name]; }
	bool Init();
private:
	std::map<std::string, TextureShader*> m_mapShader;
};

