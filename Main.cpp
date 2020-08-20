#include "Window.h"
#include "GraphicalObject.h"
#include"Entity.h"
#include <chrono> 

using namespace std::chrono;

void Draw(Window* wnd, GraphicalObject* obj, std::vector<GraphicalObject> Blocks);


int main()
{
	std::string map;
	int NrBlocksX = 64;
	float BlockScale = 2.0f/ NrBlocksX;

	Window* wnd = new Window(1600, 900,L"okno",L"HUJ");
	wnd->InitDx11();


	ImageFile* img2 = new ImageFile(L"D:\\C++\\Programy C++\\GameFramework\\Mario.png");
	ImageFile* img = new ImageFile(L"D:\\C++\\Programy C++\\GameFramework\\Blocks.png");
	std::vector<GraphicalObject> objs;
	float offsetY = 0;
		for (int i = 0; i < 33; i++)
		{
			objs.emplace_back(wnd->pDx11, img, -0.97 + i * BlockScale * 2, offsetY, BlockScale, BlockScale, 0);
			objs[i].SetUVcord(272, 287, 112, 127);
		}

	Entity Mario(wnd->pDx11, img2, -0.5, -0.5, BlockScale, BlockScale, 0);
	//Mario.SetUVcord(81,96,162,178);
	Mario.SetUVcord(96, 81, 162, 178);


	auto last = high_resolution_clock::now();
	while (wnd->ProcessMessages()==0)
	{
		const auto old = last;
		last = steady_clock::now();
		const duration<float> frameTime = last - old;
		Mario.UpdatePos(wnd, frameTime.count());
		Draw(wnd,&Mario,objs);
	}


	delete img;
	delete img2;
}

void Draw(Window* wnd,GraphicalObject* Mario, std::vector<GraphicalObject> Blocks)
{
	wnd->BeginFrame();

	for (auto& p : Blocks)
	{
		p.Draw();
	}
	Mario->Draw();
	wnd->EndFrame();
}
	