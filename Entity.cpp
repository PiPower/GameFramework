#include "Entity.h"

void Entity::UpdatePos(Window* wnd)
{
     if (wnd->IsKeyPressed('D')) Move(VelX);
	 if (wnd->IsKeyPressed('A')) Move(-VelX);
	 if (wnd->IsKeyPressed('W')) Move(0, VelY*0.67);
	 if (wnd->IsKeyPressed('S')) Move(0, -VelY*0.67);
			//if (e.Code == 'Q') Scale(0.9, 0.9);
			//if (e.Code == 'E') Scale(1.2, 1.2);

			//if (e.Code == 'X') Rotate(0.15);
			//if (e.Code == 'Z') Rotate(-0.15);

}
