#include "Window.h"
#include "GraphicalObject.h"
void Draw(Window* wnd, GraphicalObject* obj);
int main()
{
	Window* wnd = new Window(1600, 900,L"okno",L"HUJ");
	wnd->InitDx11();
	ImageFile* img = new ImageFile(L"D:\\C++\\Programy C++\\GameFramework\\Blocks.png");
	GraphicalObject obj(wnd->pDx11,0,0,0.2f,0.2f,0 ,img);
	int lul = sizeof(obj);
	obj.SetUVcord(0.3,0.5,0.3,0.5);
	while (wnd->ProcessMessages()==0)
	{
		Draw(wnd,&obj);
	}
}

void Draw(Window* wnd,GraphicalObject* obj)
{
	wnd->BeginFrame();
	obj->Draw();
	wnd->EndFrame();
}
	