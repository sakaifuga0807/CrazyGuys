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
	/*//
	virtual void HandleInput() = 0;
	virtual void ApplyPhysics() = 0;*/

	//アニメーションの初期化。
	void InitAnimation(Skeleton& skeleton, AnimationClip* clips, int numClips)
	{
		m_animation.Init(skeleton, clips, numClips);
	}

	//アニメーションの再生。
	void PlayrAnimation(int clipNo, float blendTime = 0.1f)
	{
		m_animation.Play(clipNo, blendTime);
		m_currentCllip = clipNo;
	}

protected:
	Vector3 m_velocity = Vector3::Zero;
	int m_playerID = 0;//プレイヤー識別用。

	float m_moveSpeed = 0.0f;
	float m_jumpPower = 0.0f;
	float m_gravity = 0.0f;

	Animation m_animation;
	int m_currentCllip = -1;
};

