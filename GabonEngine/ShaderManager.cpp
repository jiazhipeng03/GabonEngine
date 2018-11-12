#include "ShaderManager.h"
#include "MainApp.h"
#include "XMLParser/rapidxml.hpp"
#include "XMLParser/rapidxml_iterators.hpp"
#include "XMLParser/rapidxml_print.hpp"
#include "XMLParser/rapidxml_utils.hpp"
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

bool ShaderManager::Init(std::string filename)
{
	using namespace rapidxml;
	file<> file(filename.c_str());
	xml_document<> doc;
	doc.parse<0>(file.data());

	xml_node<>* root = doc.first_node();
	xml_node<>* shaderNode = root->first_node("shader");
	while (shaderNode)
	{
		TextureShader* shader = new TextureShader;
		std::string shaderName = shaderNode->first_attribute()->value();
		std::string vsFile = shaderNode->first_node("vs")->first_attribute()->value();
		std::string psFile = shaderNode->first_node("ps")->first_attribute()->value();
		if (!shader->Initialize(vsFile, psFile))
		{
			return false;
		}
		m_mapShader["default"] = shader;
		shaderNode = shaderNode->next_sibling("shader");
	}

	return true;
}
