#include "stdafx.h"
#include "Barrier.h"


bool Barrier::Start()
{
	m_modelRender.Init("Assets/objectData/Barrier.tkm");

	return true;
}

Barrier::Barrier()
{

}


Barrier::~Barrier()
{

}

void Barrier::Update()
{

}

void Barrier::CreateCollision()
{

}

void Barrier::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}