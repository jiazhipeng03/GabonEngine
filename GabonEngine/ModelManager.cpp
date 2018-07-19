#include "ModelManager.h"
#include "MainApp.h"


ModelManager::ModelManager()
{
}


ModelManager::~ModelManager()
{
}

bool ModelManager::Init()
{
	// obj name, shader, buffer, material
	std::string name = "square";
	std::string shadername = "default";
	TextureShader* shader = g_App->GetShaderMan()->GetShader(shadername);
	ModelObject* obj = new ModelObject;
	obj->Init(name, shader);
	m_ModelList.push_back(obj);
	return true;
}

void ModelManager::Render()
{
	for (auto m : m_ModelList)
	{
		m->Render();
		
		m->GetShader()->Render(g_App->GetDeviceContext(), m->GetVertexCount(), m->GetStartVertexIndex(),
			m->GetWorldMatrix(), m->GetDiffuseSRV());
	}
}
