#include "Window.h"
#include "GraphicalObject.h"
#include"Entity.h"
void Draw(Window* wnd, GraphicalObject* obj, GraphicalObject* Blocks);

int main()
{
	 float BlockScale = 0.05;

	Window* wnd = new Window(900, 900,L"okno",L"HUJ");
	wnd->InitDx11();
	ImageFile* img = new ImageFile(L"D:\\C++\\Programy C++\\GameFramework\\Blocks.png");
	ImageFile* img2 = new ImageFile(L"D:\\C++\\Programy C++\\GameFramework\\Mario.png");
/*	std::vector<GraphicalObject> objs;
	for (int i = 0; i < 20; i++)
	{
		objs.emplace_back(wnd->pDx11, img, 0, -0, BlockScale, BlockScale, 0);
		objs[i].SetUVcord(272, 287, 112, 127);
	}*/

	GraphicalObject objs(wnd->pDx11, img);
	//objs.SetUVcord(272, 287, 112, 127);
	Entity Mario(wnd->pDx11, img2, -0.5, -0.5, BlockScale, BlockScale, 0);
	Mario.SetUVcord(81,96,162,178);

	while (wnd->ProcessMessages()==0)
	{
		Mario.UpdatePos(wnd);
		Draw(wnd,&Mario, &objs);
	}

	delete img;
	delete img2;
}

void Draw(Window* wnd,GraphicalObject* Mario,GraphicalObject* Blocks)
{
	wnd->BeginFrame();

	Mario->Draw();
	//Blocks->Draw();

	wnd->EndFrame();
}
	