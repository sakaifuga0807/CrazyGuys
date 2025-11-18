#include "stdafx.h"
#include "PlayerControl.h"
#include "Player.h"


PlayerControl::PlayerControl(Player* player) :m_player(player)
{

}

void PlayerControl::Update()
{
	//コントローラーの入力を受け取る。
	float stickx = g_pad[0]->GetLStickXF();
	float sticky = g_pad[0]->GetLStickYF();

	//入力があれば処理をする。
	if (fabsf(stickx) > 0.001f || fabsf(sticky) > 0.001f)
	{
		Vector3 forward = g_camera3D->GetForward();
		Vector3 right = g_camera3D->GetRight();

		forward.y = 0.0f;
		right.y = 0.0f;
		forward.Normalize();
		right.Normalize();

		m_moveDir = (forward * sticky + right * stickx);
		m_moveDir.Normalize();
	}
	else
	{
		m_moveDir = Vector3::Zero;
	}

	m_jumpRequested = g_pad[0]->IsTrigger(enButtonA);
}