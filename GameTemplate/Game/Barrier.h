#pragma once

#include "Obstacle.h"

class Barrier:public Obstacle
{
public:
	bool Start()override;
	Barrier();
	~Barrier();
	void Update()override;
	//回転。
	void Rotation();
	//コリジョン。
	void CreateCollision();
	void Render(RenderContext& rc);
};

