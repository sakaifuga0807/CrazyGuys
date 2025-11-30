#pragma once

#include "Obstacle.h"
#include "PhysicsDynamicObject.h"

class Axe: public Obstacle
{
public:
	bool Start()override;
	Axe();
	~Axe();
	void Update()override;
	void UpdateAxeMotion();
	void Render(RenderContext& rc);

	void SetSpeed(float s)
	{
		m_speed = s;
	}

	void SetRange(float r)
	{
		m_range = r;
	}

private:
	//メンバ変数。
	PhysicsDynamicObject	m_physicsDynamicObject;		//物理動的オブジェクト。
	float					m_range = 0.0f;				//移動範囲。
	float					m_currentAngle = 0.0f;		//現在の角度。
	float					m_elapsedTime = 0.0f;       //経過時間。
	float					m_phase = 0.0f;             //位相。
};