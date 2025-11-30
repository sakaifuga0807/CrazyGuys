#include "stdafx.h"
#include "Axe.h"


bool Axe::Start()
{
	//モデルを読み込む。
	m_modelRender.Init("Assets/objectData/Axe.tkm");

	//物理オブジェクトを作成する。
	m_physicsDynamicObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());

	//開始方向を毎回変える。
	m_phase = Math::PI2 * (rand() / (float)RAND_MAX);

	return true;
}

Axe::Axe()
{

}

Axe::~Axe()
{

}

void Axe::Update()
{
	UpdateAxeMotion();
}

void Axe::UpdateAxeMotion()
{
	m_elapsedTime += g_gameTime->GetFrameDeltaTime();

	//経過時間を使って動作を更新する。
	float angleDeg = sinf((m_elapsedTime * m_speed) + m_phase) * m_range;
	float angleRad = Math::DegToRad(angleDeg);
	m_currentAngle = angleDeg;

	//回転。
	Quaternion rot;
	rot.SetRotationZ(angleRad);

	//物理オブジェクトの位置と回転を設定する。
	m_physicsDynamicObject.SetPosition(m_position);
	m_physicsDynamicObject.SetRotation(rot);

	//モデルの位置と回転を設定する。
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(rot);
	m_modelRender.Update();
}

void Axe::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}