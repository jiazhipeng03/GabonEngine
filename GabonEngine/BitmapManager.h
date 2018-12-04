#pragma once
#include "Bitmap.h"
class BitmapManager
{
public:
	BitmapManager();
	~BitmapManager();
	bool Init(std::string fileName);
	void Render();
private:
	std::vector<Bitmap*> m_ModelList;
};