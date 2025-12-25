#include "stdafx.h"
#include "PlayerControl.h"
#include "Player.h"

namespace
{
	constexpr int MAX_PAD = 4;//最大接続コントローラー数
}

PlayerControl::PlayerControl(Player* player,int controllerIndex)
	: m_player(player)
	,m_controllerIndex(controllerIndex)
{

}

void PlayerControl::Update()
{
	//コントローラー番号が不正なら、入力は受け付けない。
	if (m_controllerIndex < 0 || m_controllerIndex >= MAX_PAD)
	{
		m_moveDir = Vector3::Zero;
		m_jumpRequested = false;

		return;
	}

	/*//コントローラーが接続されていなければ、入力は受け付けない。
	if (!g_pad[m_controllerIndex] || !g_pad[m_controllerIndex]->IsConnected())
	{
		m_moveDir = Vector3::Zero44;
		m_jumpRequested = false;

		return;
	}*/

	// コントローラーの入力を受け取る。
	float stickx = g_pad[m_controllerIndex]->GetLStickXF();
	float sticky = g_pad[m_controllerIndex]->GetLStickYF();

	//入力があれば処理をする。
	if (fabsf(stickx) > 0.001f || fabsf(sticky) > 0.001f)
	{
		Vector3 forward = g_camera3D->GetForward();
		Vector3 right = g_camera3D->GetRight();

		forward.y = 0.0f;
		right.y = 0.0f;
		forward.Normalize();
		right.Normalize();

		m_moveDir = forward * sticky + right * stickx;
		m_moveDir.Normalize();
	}
	else
	{
		m_moveDir = Vector3::Zero;
	}

	m_jumpRequested = g_pad[m_controllerIndex]->IsTrigger(enButtonA);
}