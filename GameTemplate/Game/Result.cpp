#include "stdafx.h"
#include "Result.h"
#include "Title.h"

bool Result::Start()
{
	m_spriteRender.Init("Assets/sprite/White.dds", 1920.0f,1080.0f);
	m_spriteRender.Update();

	return true;
}

Result::Result()
{

}

Result::~Result()
{

}

void Result::Update()
{
	if (g_pad[0]->IsTriggerAnyKey())
	{
		m_title = NewGO<Title>(0, "title");
		DeleteGO(this);
	}
}

void Result::Render(RenderContext& rc)
{
	m_spriteRender.Draw(rc);
}