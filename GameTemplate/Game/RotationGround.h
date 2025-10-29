#pragma once

#include "Obstacle.h"
#include "PhysicsDynamicObject.h"

class Player;

class RotationGround :public Obstacle
{
public:
	bool Start()override;
	RotationGround();
	~RotationGround();
	void Update()override;
	void Render(RenderContext& rc);
	void SetAngleSpeed(float speed)
	{
		m_angleSpeed = speed;
	}

private:
	PhysicsDynamicObject	m_physicsDynamicObj;				//静的動的オブジェクト。
	float					m_angleSpeed = 0.0f;				//回転速度。
};