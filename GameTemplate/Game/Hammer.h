#pragma once

#include "Obstacle.h"

class Hammer:public Obstacle
{
public:
	bool Start()override;
	Hammer();
	~Hammer();
	void Update()override;
	//回転。
	void Rotation();
	//コリジョンを作成
	void CreateCollision();
	void Render(RenderContext& rc);	

private:
	//メンバ変数。
	float		m_rotationSpeed = 0.0f;					//回転速度。
};