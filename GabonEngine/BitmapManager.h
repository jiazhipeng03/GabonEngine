#pragma once
#include "Bitmap.h"
class BitmapManager
{
public:
	BitmapManager();
	~BitmapManager();
	bool Init(std::string fileName);
	void Render();
	Bitmap* GetBitmap(int i);
private:
	std::vector<Bitmap*> m_ModelList;
};
