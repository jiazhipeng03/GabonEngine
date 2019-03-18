#include "ModelManager.h"
#include "MainApp.h"
#include "Mirror.h"
#include "XMLParser/rapidxml.hpp"
#include "XMLParser/rapidxml_utils.hpp"
#include "XMLParserHelper.h"

ModelManager::ModelManager()
{
}


ModelManager::~ModelManager()
{
	for (auto m : m_ModelList)
	{
		SafeDelete(m);
	}
	m_ModelList.clear();
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
		//测试mirror，以后需要重构mirror的初始化和渲染流程，破坏了当前渲染结构
// 		if (modelName == "mirror")
// 		{
// 			obj = new Mirror;
// 		}
// 		else
// 		{
// 			obj = new ModelObject;
// 		}
		Vector3 position(0, 0, 0);
		if(modelNode->first_node("position"))
			position = XMLParserHelper::ParseVec3(modelNode->first_node("position")->first_attribute()->value());
		std::string meshName = modelNode->first_node("mesh")->first_attribute()->value();
		std::string shaderName = modelNode->first_node("shader")->first_attribute()->value();
		TextureShader* shader = g_App->GetShaderMan()->GetShader(shaderName);
		xml_node<>* texNode = modelNode->first_node("texture");
		std::vector<std::string> texNames;
		while (texNode)
		{
			std::string texName = texNode->first_attribute()->value();
			texNames.push_back(texName);
			texNode = texNode->next_sibling("texture");
		}
		if (!shader)
		{
			char str[256];
			sprintf_s(str, "ModelManager::Init obj %s shader %s not exists", meshName.c_str(), shaderName.c_str());
			OutputDebugStringA(str);
			modelNode = modelNode->next_sibling("model");
			continue;
		}
		obj->Init(modelName, shader, meshName, texNames);
		obj->SetPosition(position);
		m_ModelList.push_back(obj);
		modelNode = modelNode->next_sibling("model");
	}
	doc.clear();
	
	return true;
}

void ModelManager::Render(class Frustum* frustum)
{
	for (auto m : m_ModelList)
	{
		// check in frustum
		//Vector3 pos = m->GetWorldMatrix().getTrans();
		//bool bNeedRender = frustum->CheckSphere(pos);
		//if(bNeedRender)
		{
			m->Render();

			m->GetShader()->Render(g_App->GetDeviceContext(), m->GetVertexCount(), m->GetStartVertexIndex(),
				m->GetWorldMatrix(), g_App->GetCamera()->Proj(), m->GetTexArray());
		}
	}
}

ModelObject* ModelManager::GetModel(std::string name)
{
	for (auto m : m_ModelList)
	{
		if (m->GetName() == name)
		{
			return m;
		}
	}
	return NULL;
}

ModelObject* ModelManager::CloneModelObj(const char* srcName)
{
	ModelObject* src = GetModel(srcName);
	ModelObject* obj = new ModelObject;
	std::string name;
	name = srcName + std::string("_clone");
	obj->Init(name, src->GetShader(), src->GetMeshName(), obj->GetTexNames());
	m_ModelList.push_back(obj);
	return obj;
}
