#pragma once

class Player;

class PlayerControl:public IGameObject
{
public:
	PlayerControl(Player* player,int controllerIndex);

	void Update();

	const Vector3& GetMoveDir()const
	{
		return m_moveDir;
	}

	bool IsJumpRequested()const
	{
		return m_jumpRequested;
	}

private:
	Player* m_player = nullptr;
	Vector3 m_moveDir = Vector3::Zero;
	int m_controllerIndex = 0;
	bool m_jumpRequested = false;
};