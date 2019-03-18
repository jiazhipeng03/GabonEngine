#pragma once
#include "ModelObject.h"
class ModelManager
{
public:
	ModelManager();
	~ModelManager();
	bool Init(std::string fileName);
	void Render(class Frustum* frustum);
	ModelObject* GetModel(std::string name);
	ModelObject* CloneModelObj(const char* srcName);
private:
	std::vector<ModelObject*> m_ModelList;
	
};

