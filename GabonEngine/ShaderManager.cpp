#include "ShaderManager.h"
#include "MainApp.h"


ShaderManager::ShaderManager()
{
}


ShaderManager::~ShaderManager()
{
	for (auto shader : m_mapShader)
	{
		SafeDelete(shader.second);
	}
}

bool ShaderManager::Init()
{
	TextureShader* shader = new TextureShader;
	shader->Initialize(g_App->GetDevice(), g_App->GetWindowHandle(), "texture.vs", "texture.ps");
	m_mapShader["default"] = shader;

	return true;
}
