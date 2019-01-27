#pragma once
#include "ModelObject.h"
class ModelManager
{
public:
	ModelManager();
	~ModelManager();
	bool Init(std::string fileName);
	void Render(class Frustum* frustum);
private:
	std::vector<ModelObject*> m_ModelList;
};

