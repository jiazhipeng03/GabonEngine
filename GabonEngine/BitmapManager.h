#pragma once
#include "Bitmap.h"
class BitmapManager
{
public:
	BitmapManager();
	~BitmapManager();
	bool Init(std::string fileName);
	void Render();
	Bitmap* GetBitmap(ui32 i);
	Bitmap* CreateBitmap();
private:
	std::vector<Bitmap*> m_ModelList;
};
