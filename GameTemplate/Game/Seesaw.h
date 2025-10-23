#pragma once

#include "Obstacle.h"
#include "PhysicsDynamicObject.h"

class Player;

class Seesaw :public Obstacle
{
public:
	enum enMovingSeesaw
	{
		enUp,
		enDown,
	};

	bool Start()override;
	Seesaw();
	~Seesaw();
	void Update()override;
	//回転。
	void Rotation();
	//プレイヤーが乗っているか判定。
	void CheckPlayerOn();
	void Render(RenderContext& rc);

private:
	Player* m_player=nullptr;//プレイヤーのポインタ。
	PhysicsDynamicObject m_physicsDynamicObj;//静的動的オブジェクト。
	enMovingSeesaw m_movingSeesawState = enUp;//シーソーの傾き状態。
	float m_angleSpeed = 30.0f;//回転速度。
	float m_limitAngle = 15.0f;//傾く最大角度。
	float m_currentAngle = 0.0f;//現在の角度。
};