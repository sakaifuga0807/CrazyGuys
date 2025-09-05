#pragma once
#include "Actor.h"

class ICharacter :public Actor
{
public:
	bool Start()override;
	ICharacter();
	virtual~ICharacter();
	void Update()=0;
	void Render(RenderContext& rc)override;

	//プレイヤーのIDを設定する。
	void SetPlayerID(int id)
	{
		m_playerID = id;
	}

	//プレイヤーのIDを取得する。
	int GetPlayerID()const
	{
		return m_playerID;
	}

protected:
	int m_playerID = 0;//プレイヤーID。
};

