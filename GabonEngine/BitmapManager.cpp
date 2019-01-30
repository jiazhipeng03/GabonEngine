#include "BitmapManager.h"
#include "MainApp.h"
#include "XMLParser/rapidxml.hpp"
#include "XMLParser/rapidxml_utils.hpp"
#include "XMLParserHelper.h"

BitmapManager::BitmapManager()
{
}


BitmapManager::~BitmapManager()
{
	for (auto m : m_ModelList)
	{
		SafeDelete(m);
	}
	m_ModelList.clear();
}

bool BitmapManager::Init(std::string fileName)
{	
	using namespace rapidxml;
	file<> file(fileName.c_str());
	xml_document<> doc;
	doc.parse<0>(file.data());
	
	xml_node<>* root = doc.first_node();
	xml_node<>* modelNode = root->first_node("bitmap");
	while (modelNode)
	{
		Bitmap* obj = new Bitmap;		
		std::string modelName = modelNode->first_attribute()->value();
		//std::string meshName = modelNode->first_node("mesh")->first_attribute()->value();
		std::string shaderName = modelNode->first_node("shader")->first_attribute()->value();
		xml_node<>* texNode = modelNode->first_node("texture");
		std::vector<std::string> texNames;
		while (texNode)
		{
			std::string texName = texNode->first_attribute()->value();
			texNames.push_back(texName);
			texNode = texNode->next_sibling("texture");
		}		
		std::string texPos = modelNode->first_node("position")->first_attribute()->value();
		Vector2 pos = XMLParserHelper::ParseVec2(texPos);
		std::string texSize = modelNode->first_node("size")->first_attribute()->value();
		Vector2 size = XMLParserHelper::ParseVec2(texSize);
		TextureShader* shader = g_App->GetShaderMan()->GetShader(shaderName);
		assert(shader);
		obj->Init(pos, size, g_App->GetScreenSize(), texNames/*"Texture/seafloor.dds"*/, shader);
		m_ModelList.push_back(obj);
		modelNode = modelNode->next_sibling("bitmap");
	}
	doc.clear();
	return true;
}

void BitmapManager::Render()
{
	for (auto b : m_ModelList)
	{
		b->UpdateBuffer();
		b->Render();
	}
}

Bitmap* BitmapManager::GetBitmap(ui32 i)
{
	if (i >= 0 && i < m_ModelList.size())
	{
		return m_ModelList[i];
	}
	return nullptr;
}
