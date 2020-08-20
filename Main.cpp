#include "Window.h"
#include "GraphicalObject.h"
#include"Entity.h"
void Draw(Window* wnd, GraphicalObject* obj);
int main()
{
	Window* wnd = new Window(900, 900,L"okno",L"HUJ");
	wnd->InitDx11();
	ImageFile* img = new ImageFile(L"D:\\C++\\Programy C++\\GameFramework\\Blocks.png");
	GraphicalObject obj(wnd->pDx11,img,0,0,0.2,0.2);
	int lul = sizeof(obj);
	//obj.Move(0.2, 0.2);
	//obj.Rotate(3.14/2.0);
	//obj.Scale(1.2);
	obj.SetUVcord(272.0f/448.0f, 287.0f / 448.0f,112.0f/256.0f, 127.0f / 256.0f);
	while (wnd->ProcessMessages()==0)
	{
		while (!wnd->IsKeyboardEventEmpty())
		{
			const auto e = wnd->ReadKeyEvent();
			if (e.Type == Window::KeyEvent::Event::Press)
			{
				if (e.Code == 'D')
				{
					obj.Move(0.07);
				}
				if (e.Code == 'A') obj.Move(-0.07);
				if (e.Code == 'W') obj.Move(0,0.7);
				if (e.Code == 'S')  obj.Move(0, -0.7);

				if (e.Code == 'Q')  obj.Scale(0.9,0.9);
				if (e.Code == 'E')  obj.Scale(1.2,1.2);

				if (e.Code == 'X')  obj.Rotate(0.15);
				if (e.Code == 'Z')  obj.Rotate(-0.15);
			}
		}
		Draw(wnd,&obj);
	}
}

void Draw(Window* wnd,GraphicalObject* obj)
{
	wnd->BeginFrame();
	obj->Draw();
	wnd->EndFrame();
}
	