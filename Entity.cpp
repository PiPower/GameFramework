#include "Entity.h"
#include <functional>
#include <cmath>
#include <algorithm>

using namespace DirectX;

void Entity::UpdatePos(Window* wnd,float Time, Camera& cam,std::vector<GraphicalObject*>& Blocks)
{
	MoveVec = { 0,-0.6f };


    if (wnd->IsKeyPressed('D')) MoveVec.x += VelX;
	if (wnd->IsKeyPressed('A')) MoveVec.x += -VelX ;
	if (wnd->IsKeyPressed('W'))MoveVec.y += VelY;
	if (wnd->IsKeyPressed('S')) MoveVec.y +=-VelY;
	if (wnd->IsKeyPressed(VK_SPACE)&& !Jump)
	{
		JumpFactor = 1.0f;
		Jump = true;
	}

	MoveVec.y += 3*VelY*JumpFactor;
	if (JumpFactor != 0) JumpFactor -= 0.1f;
    //Move(MoveVec.x, MoveVec.y);
   // Collision Stuff --------------------------------------------------
	auto lel = GetVertecies();

	XMFLOAT2 contact_point;
	XMFLOAT2 contact_normal{0,0};
	float contact_time;

	XMFLOAT2 senVel = { MoveVec.x * Time,MoveVec.y * Time };
	std::vector<std::pair<int, float>> vec;
	for (int i=0 ; i< Blocks.size(); i++)
	{
		if (DynamicRectVsRect(lel, senVel, Blocks[i]->GetVertecies(), contact_point, contact_normal, contact_time))
		{
			vec.push_back({ i,contact_time });
		}
	}

	std::sort(vec.begin(), vec.end(), [](const std::pair<int, float>& a,const std::pair<int, float>& b) 
		{return a.second < b.second; }
	);

	for (int i = 0; i < vec.size(); i++)
	{
		if (DynamicRectVsRect(lel, MoveVec, Blocks[vec[i].first]->GetVertecies(), contact_point, contact_normal, contact_time))
		{
			MoveVec.x += contact_normal.x * (1.0f-contact_time) * abs(MoveVec.x);
		    MoveVec.y += contact_normal.y * (1.0f-contact_time) * abs(MoveVec.y);

			if (contact_normal.y == 1)
			{
				JumpFactor = 0;
				Jump = false;
			}
		}
	}


	Move(MoveVec.x*Time, MoveVec.y*Time);

	auto PlayerRect = GetVertecies();
	float Time2 = MoveVec.y * Time;
	if (PlayerRect.TopLeft.x + cam.OffsetX < -0.3 || PlayerRect.BottomRight.x + cam.OffsetX > 0.3)  cam.UpdateOffsets(-MoveVec.x * Time,0);


	// Animation Stuff-------------------------------------------------
	if (MoveVec.x == 0 && MoveVec.y * Time <= 0 && MoveVec.y * Time>=-0.01f)
	{
		if(LastDir == LastDirection::Right)SetUVcord(81, 96, 162, 178);
		if(LastDir == LastDirection::Left)SetUVcord(96, 81, 162, 178);
		LastDir = LastDirection::Standing;
	}

	if (MoveVec.x > 0)
	{
		if (LastDir == LastDirection::Right)
		{
			Timer += Time;

			if (Timer >= FrameChange * 5) Timer = 0;

			if (Timer >= FrameChange * 4)SetUVcord(148, 164, 162, 178);

		    else if(Timer >= FrameChange * 3) SetUVcord(131, 147, 162, 178);
		    
			else if (Timer >= FrameChange * 2)SetUVcord(114, 130, 162, 178);

			else if (Timer >= 0.0)	SetUVcord(97, 113, 162, 178);
			
		}
		else  Timer = 0;
	

		LastDir = LastDirection::Right;
	}

	if (MoveVec.x < 0)
	{
		if (LastDir == LastDirection::Left)
		{
			Timer += Time;

			if (Timer >= FrameChange * 4) Timer = 0;

			if (Timer >= FrameChange * 4)SetUVcord(164, 148, 162, 178);

			else if (Timer >= FrameChange * 3)SetUVcord(147, 131, 162, 178);

			else if (Timer >= FrameChange * 2)SetUVcord(130, 114, 162, 178);

			else if (Timer >= 0.0)	SetUVcord(113, 97, 162, 178);

		}
		else  Timer = 0;

		LastDir = LastDirection::Left;
	}

}

bool Entity::RectVsRey(const DirectX::XMFLOAT2& RayOrigin, const DirectX::XMFLOAT2& RayDir, const CollRect& rect,
     DirectX::XMFLOAT2& ContactPoint,  DirectX::XMFLOAT2& ContactNormal, float& t_hit_near)
{
	XMFLOAT2 t_near{ (rect.TopLeft.x-RayOrigin.x)/ RayDir.x,(rect.TopLeft.y - RayOrigin.y) / RayDir.y };
	XMFLOAT2 t_far{ (rect.BottomRight.x - RayOrigin.x) / RayDir.x,(rect.BottomRight.y - RayOrigin.y) / RayDir.y };

	if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
	if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);

	if (std::isnan(t_far.y) || std::isnan(t_far.x)) return false;
	if (std::isnan(t_near.y) || std::isnan(t_near.x)) return false;


	if (t_near.x > t_far.y || t_near.y > t_far.x) return false;

#ifdef max
#undef max
#endif 

#ifdef min
#undef min
#endif 

    t_hit_near = std::max(t_near.x, t_near.y);
	float t_hit_far = std::min(t_far.x, t_far.y);
	 
	if (t_hit_far < 0) return false;

	ContactPoint = { RayOrigin.x + t_hit_near * RayDir.x,RayOrigin.y + t_hit_near * RayDir.y };

	if (t_near.x > t_near.y)
		if (RayDir.x < 0)
			ContactNormal = { 1,0 };
		else
			ContactNormal = { -1,0 };
	else if (t_near.x < t_near.y)
		if (RayDir.y < 0)
			ContactNormal = { 0,1 };
		else
			ContactNormal = { 0,-1 };
	
	return true;
}

bool Entity::DynamicRectVsRect(const CollRect& in, DirectX::XMFLOAT2& Velocity, const CollRect& target, 
	DirectX::XMFLOAT2& contact_point, DirectX::XMFLOAT2& contact_normal, float& contact_time)
{
	if (Velocity.x == 0 && Velocity.y == 0)
		return false;

	CollRect Expanded{ {0,0},{0,0} };

	XMFLOAT2 SizeIn{ abs(in.BottomRight.x - in.TopLeft.x),abs(in.BottomRight.y - in.TopLeft.y) };

	float x = target.TopLeft.x - SizeIn.x / 2.0f;
	Expanded.TopLeft = { target.TopLeft.x - SizeIn.x / 2.0f,target.TopLeft.y + SizeIn.y / 2.0f };
	Expanded.BottomRight.x =  target.BottomRight.x + SizeIn.x / 2.0f ;
	Expanded.BottomRight.y = target.BottomRight.y - SizeIn.y / 2.0f;



	if (RectVsRey(XMFLOAT2{ in.TopLeft.x+ SizeIn.x/2.0f, in.TopLeft.y - SizeIn.y / 2.0f }, Velocity, Expanded,contact_point,contact_normal, contact_time))
	{
		if (contact_time <= 1.0f)
		{
			return true;
		}
	}

	return false;
}

