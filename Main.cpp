#include "Window.h"
#include "GraphicalObject.h"
#include"Entity.h"
#include <chrono> 
#include "BoardMenager.h"
using namespace std::chrono;

void Draw(Window* wnd, GraphicalObject* obj, std::vector<GraphicalObject* >& Blocks,BoardMenager* brd);


int main()
{
	std::string map;
	int NrBlocksX = 32;
	float BlockScale = 1.0f/ NrBlocksX;

	Window* wnd = new Window(1600, 900,L"okno",L"HUJ");
	wnd->InitDx11();


	std::wstring pathMario=L"D:\\C++\\Programy C++\\GameFramework\\Mario.png";
	std::wstring pathBlocks = L"D:\\C++\\Programy C++\\GameFramework\\Blocks.png";
	std::vector<GraphicalObject* > objs;

	   float offsetY = 0;
		for (int i = 0; i < 1; i++)
		{
			objs.emplace_back(new GraphicalObject(wnd->pDx11, pathBlocks, -0.97 + (i+1) * BlockScale * 2,0.032, BlockScale, BlockScale, 0));
			objs[i]->SetUVcord(272, 287, 112, 127);
		}
		
	Entity Mario(wnd->pDx11, pathMario, -0.5, -0.5, BlockScale, BlockScale, 0);
	Mario.SetUVcord(81,96,162,178);

	BoardMenager Menager(wnd->pDx11, BlockScale);


	auto last = high_resolution_clock::now();
	while (wnd->ProcessMessages()==0)
	{
		const auto old = last;
		last = steady_clock::now();
		const duration<float> frameTime = last - old;

		Mario.UpdatePos(wnd, frameTime.count());

		Menager.AddBlocks(objs, wnd, pathBlocks);

		Draw(wnd,&Mario,objs,&Menager);

	}

	for (auto& p : objs)
	{
		delete p;
	}
}

void Draw(Window* wnd,GraphicalObject* Mario, std::vector<GraphicalObject*>& Blocks, BoardMenager* brd)
{
	wnd->BeginFrame();

	for (auto& p : Blocks)
	{
		p->Draw();
	}

    brd->Draw();


	//Mario->Draw();
	wnd->EndFrame();
}
	