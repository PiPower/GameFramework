#include "Window.h"
#include "GraphicalObject.h"
#include"Entity.h"
#include <chrono> 
#include "BoardMenager.h"
#include "Camera.h"

using namespace std::chrono;

void Draw(Window* wnd, GraphicalObject* obj, BoardMenager* brd, Camera* cam);

bool CreationMode = true;
int main()
{
	bool Save = false;
	bool Load = true;
	int NrBlocksX = 40;
	float BlockScale = 1.0f/ NrBlocksX;

	Window* wnd = new Window(1600, 900,L"okno",L"HUJ");
	wnd->InitDx11();

	Camera cam(wnd->pDx11,0.2, 0.2);

	std::wstring pathMario=L"D:\\C++\\Programy C++\\GameFramework\\Mario.png";
	std::wstring pathBlocks = L"D:\\C++\\Programy C++\\GameFramework\\Blocks.png";

	Entity Mario(wnd->pDx11, pathMario,0, 0.3, BlockScale*0.90, BlockScale * 0.90, 0);
	Mario.SetUVcord(83,94,162,178);

	BoardMenager Menager(wnd->pDx11, BlockScale);
	if(Load)Menager.LoadBoard("Blocks.txt", pathBlocks);

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

		if (CreationMode)
		{
			if (wnd->IsKeyPressed(VK_UP)) cam.UpdateOffsets(0, -BlockScale * 2);
			if (wnd->IsKeyPressed(VK_DOWN)) cam.UpdateOffsets(0, BlockScale * 2);
			if (wnd->IsKeyPressed(VK_RIGHT)) cam.UpdateOffsets(-BlockScale * 2, 0);
			if (wnd->IsKeyPressed(VK_LEFT)) cam.UpdateOffsets(BlockScale * 2, 0);

			Menager.AddBlocks( wnd, pathBlocks, cam);
		}
		else Mario.UpdatePos(wnd, frameTime.count(), cam, Menager.GetBlocks());

			Draw(wnd, &Mario, &Menager, &cam);
		

	}

	if(Save) Menager.SaveBoard("Blocks.txt");
}

void Draw(Window* wnd,GraphicalObject* Mario, BoardMenager* brd,Camera* cam)
{
	wnd->BeginFrame();

	cam->BindConstBuffer();

	brd->DrawBlocks();

	if(CreationMode)brd->Draw();

	Mario->Draw();

	wnd->EndFrame();
}
	