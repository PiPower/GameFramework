#include "Entity.h"

void Entity::UpdatePos(Window* wnd,float Time, Camera& cam)
{
	float VecX=0;
	float VecY = 0;
    if (wnd->IsKeyPressed('D')) VecX = VelX* Time;
	if (wnd->IsKeyPressed('A')) VecX =-VelX * Time;
	if (wnd->IsKeyPressed('W')) VecY = VelY* Time;
	if (wnd->IsKeyPressed('S')) VecY =-VelY* Time;

	Move(VecX, VecY);
   // Collision Stuff --------------------------------------------------
	auto lel = GetVertecies();
	bool ret = false;
	for (int i = 0; i < 4; i++)
	{
		if (lel[i].x <= -1) ret = true;
	}

	if (ret) Move(-VecX, -VecY);


	if (lel[0].x +cam.OffsetX < -0.3 || lel[1].x + cam.OffsetX> 0.3) cam.UpdateOffsets(-VecX);

	// Animation Stuff-------------------------------------------------
	if (VecX == 0 && VecY == 0)
	{
		if(LastDir == LastDirection::Right)SetUVcord(81, 96, 162, 178);
		if(LastDir == LastDirection::Left)SetUVcord(96, 81, 162, 178);
		LastDir = LastDirection::Standing;
	}

	if (VecX > 0)
	{
		if (LastDir == LastDirection::Right)
		{
			Timer += Time;

			if (Timer >= FrameChange * 4) Timer = 0;

		    if(Timer >= FrameChange * 3)SetUVcord(131, 147, 162, 178);
		    
			else if (Timer >= FrameChange * 2)SetUVcord(114, 130, 162, 178);

			else if (Timer >= 0.0)	SetUVcord(97, 113, 162, 178);
			
		}
		else  Timer = 0;
	

		LastDir = LastDirection::Right;
	}

	if (VecX < 0)
	{
		if (LastDir == LastDirection::Left)
		{
			Timer += Time;

			if (Timer >= FrameChange * 4) Timer = 0;

			if (Timer >= FrameChange * 3)SetUVcord(147, 131, 162, 178);

			else if (Timer >= FrameChange * 2)SetUVcord(130, 114, 162, 178);

			else if (Timer >= 0.0)	SetUVcord(113, 97, 162, 178);

		}
		else  Timer = 0;

		LastDir = LastDirection::Left;
	}

}

void Entity::Collision()
{
	HRESULT hr;
}
