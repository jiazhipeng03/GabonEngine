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

bool ShaderManager::Init()
{	
	Parse("shader.xml");

// 	TextureShader* shader = new TextureShader;
// 	shader->Initialize(g_App->GetDevice(), g_App->GetWindowHandle(), "texture.vs", "texture.ps");
// 	m_mapShader["default"] = shader;

	return true;
}

void ShaderManager::Parse(std::string filename)
{
	using namespace rapidxml;
	file<> file("shader.xml");
	xml_document<> doc;
	doc.parse<0>(file.data());

	xml_node<>* root = doc.first_node();
	xml_node<>* shaderNode = root->first_node("shader");
	while (shaderNode)
	{
		TextureShader* shader = new TextureShader;
		std::string shaderName = shaderNode->first_attribute()->value();
	}
}
