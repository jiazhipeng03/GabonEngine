#pragma once
#include "ModelObject.h"
class ModelManager
{
public:
	ModelManager();
	~ModelManager();
	bool Init();
	void Render();
private:
	std::vector<ModelObject*> m_ModelList;
};

