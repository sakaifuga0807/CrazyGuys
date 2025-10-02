#pragma once
#include "Actor.h"

class ICharacter :public Actor
{
public:
	ICharacter()=default;
	virtual~ICharacter() {};
	virtual void Update()=0;//純粋仮想関数。派生先で呼び出さないとコンパイルエラーになる。
	void Render(RenderContext& rc)override
	{
		m_modelRender.Draw(rc);
	}
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

	//アニメーションの初期化。
	void InitAnimation(Skeleton& skeleton, AnimationClip* clips, int numClips)
	{
		m_animation.Init(skeleton, clips, numClips);
	}

	//アニメーションの再生。
	void PlayAnimation(int clipNo, float blendTime = 0.1f)
	{
		m_animation.Play(clipNo, blendTime);
		m_currentClip = clipNo;
	}

protected:
	Animation		m_animation;			//アニメーション。
	int				m_playerID = 0;			//プレイヤー識別用ID。
	float			m_moveSpeed = 0.0f;		//移動速度。
	float			m_jumpPower = 0.0f;		//ジャンプ力。
	float			m_gravity = 0.0f;		//重力。
	int				m_currentClip = -1;
};

