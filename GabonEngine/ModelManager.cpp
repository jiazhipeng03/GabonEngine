#include "ModelManager.h"
#include "MainApp.h"
#include "XMLParser/rapidxml.hpp"
#include "XMLParser/rapidxml_utils.hpp"

ModelManager::ModelManager()
{
}


ModelManager::~ModelManager()
{
	for (auto m : m_ModelList)
	{
		SafeDelete(m);
	}
}

bool ModelManager::Init(std::string fileName)
{
	using namespace rapidxml;
	file<> file(fileName.c_str());
	xml_document<> doc;
	doc.parse<0>(file.data());

	xml_node<>* root = doc.first_node();
	xml_node<>* modelNode = root->first_node("model");
	while (modelNode)
	{
		ModelObject* obj = new ModelObject;
		std::string modelName = modelNode->first_attribute()->value();
		std::string meshName = modelNode->first_node("mesh")->first_attribute()->value();
		std::string shaderName = modelNode->first_node("shader")->first_attribute()->value();
		TextureShader* shader = g_App->GetShaderMan()->GetShader(shaderName);
		obj->Init(modelName, shader, meshName);
		m_ModelList.push_back(obj);
		modelNode = modelNode->next_sibling("model");
	}
	
	return true;
}

void ModelManager::Render()
{
	for (auto m : m_ModelList)
	{
		m->Render();
		m->GetShader()->Render(g_App->GetDeviceContext(), m->GetVertexCount(), m->GetStartVertexIndex(),
			m->GetWorldMatrix(), g_App->GetCamera()->Proj(), m->GetDiffuseSRV());
	}
}
