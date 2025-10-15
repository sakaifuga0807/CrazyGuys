#pragma once

#include "Actor.h"

class Obstacle:public Actor
{
public:
	Obstacle()=default;
	virtual~Obstacle()=default;
	bool Start()override;
	virtual void Update() override = 0;

protected:
	float m_angle = 0.0f;//角度。
	float m_speed = 0.0f;//速度。
};