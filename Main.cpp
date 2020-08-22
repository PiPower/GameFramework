#include "Window.h"
#include "GraphicalObject.h"
#include"Entity.h"
#include <chrono> 
#include "BoardMenager.h"
#include "Camera.h"

using namespace std::chrono;

void Draw(Window* wnd, GraphicalObject* obj, std::vector<GraphicalObject* >& Blocks,BoardMenager* brd, Camera* cam);

bool CreationMode = false;
int main()
{

	int NrBlocksX = 40;
	float BlockScale = 1.0f/ NrBlocksX;

	Window* wnd = new Window(1600, 900,L"okno",L"HUJ");
	wnd->InitDx11();
	Camera cam(wnd->pDx11,0.2, 0.2);

	std::wstring pathMario=L"D:\\C++\\Programy C++\\GameFramework\\Mario.png";
	std::wstring pathBlocks = L"D:\\C++\\Programy C++\\GameFramework\\Blocks.png";


	std::vector<GraphicalObject* > Blocks;

	   float offsetY = 0;
		for (int i = 0; i < 1; i++)
		{
			Blocks.emplace_back(new GraphicalObject(wnd->pDx11, pathBlocks, 0,0, BlockScale, BlockScale, 0));
			Blocks[i]->SetUVcord(193, 208, 1, 16);
		}
		


	Entity Mario(wnd->pDx11, pathMario,0, 0.3, BlockScale*0.90, BlockScale * 0.90, 0);
	Mario.SetUVcord(83,94,162,178);

	BoardMenager Menager(wnd->pDx11, BlockScale);


	auto last = high_resolution_clock::now();


	while (wnd->ProcessMessages()==0)
	{
		const auto old = last;
		last = steady_clock::now();
		const duration<float> frameTime = last - old;


		while (!wnd->IsKeyboardEventEmpty())
		{
			const auto e = wnd->ReadKeyEvent();
			if (e.Code == 'P' && e.Type == Window::KeyEvent::Event::Press)
			{
				CreationMode = !CreationMode;
			}
		}

		Mario.UpdatePos(wnd, frameTime.count(),cam, Blocks);
		if (CreationMode)
		{
			if (wnd->IsKeyPressed(VK_UP)) cam.UpdateOffsets(0, -BlockScale * 2);
			if (wnd->IsKeyPressed(VK_DOWN)) cam.UpdateOffsets(0, BlockScale * 2);
			if (wnd->IsKeyPressed(VK_RIGHT)) cam.UpdateOffsets(-BlockScale * 2, 0);
			if (wnd->IsKeyPressed(VK_LEFT)) cam.UpdateOffsets(BlockScale * 2, 0);

			Menager.AddBlocks(Blocks, wnd, pathBlocks, cam);
		}

		Draw(wnd,&Mario, Blocks,&Menager,&cam);

	}

	for (auto& p : Blocks)
	{
		delete p;
	}
}

void Draw(Window* wnd,GraphicalObject* Mario, std::vector<GraphicalObject*>& Blocks, BoardMenager* brd,Camera* cam)
{
	wnd->BeginFrame();

	cam->BindConstBuffer();

	for (auto& p : Blocks)
	{
		p->Draw();
	}

	if(CreationMode)brd->Draw();

	Mario->Draw();

	wnd->EndFrame();
}
	