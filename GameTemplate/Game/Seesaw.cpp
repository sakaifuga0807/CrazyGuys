#include "stdafx.h"
#include "Seesaw.h"
#include "Player.h"
#include "JsonUtility.h"


bool Seesaw::Start()
{
	m_modelRender.Init("Assets/objectData/Seesaw.tkm");

	json configData;
	//Jsonƒtƒ@ƒCƒ‹‚ð“Ç‚Ýž‚ÞB
	if (!JsonUtility::LoadJson("Assets/config/Seesaw.json", configData))
	{
		return false;
	}

	//Seesaw‚Ì’l‚ð•Ï”‚ÉŠi”[B
	auto seesawData = configData["Seesaw"];

	//‰ñ“]‘¬“xB
	m_angleSpeed = seesawData["AngleSpeed"];

	//§ŒÀŠp“xB
	m_limitAngle = seesawData["LimitAngle"];

	m_modelRender.SetPosition(m_position);

	m_physicsDynamicObj.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());

	return true;
}

Seesaw::Seesaw()
{
	
}

Seesaw::~Seesaw()
{

}

void Seesaw::Update()
{
	UpdateSeesawState();
}

void Seesaw::UpdateSeesawState()
{
	//
	if (m_movingSeesawState == enUp)
	{
		m_currentAngle += m_angleSpeed * g_gameTime->GetFrameDeltaTime();
	}
	else if(m_movingSeesawState==enDown)
	{
		m_currentAngle -= m_angleSpeed * g_gameTime->GetFrameDeltaTime();
	}
	if (m_currentAngle >= m_limitAngle)
	{
		m_currentAngle = m_limitAngle;
		m_movingSeesawState = enDown;
	}
	else if (m_currentAngle <= -m_limitAngle)
	{
		m_currentAngle = -m_limitAngle;
		m_movingSeesawState = enUp;
	}

	Quaternion rot;
	rot.SetRotationDeg(Vector3::AxisX,m_currentAngle);

	//À•W‚Æ‰ñ“]‚ð”½‰f‚³‚¹‚éB
	m_physicsDynamicObj.SetPosition(m_position);
	m_physicsDynamicObj.SetRotation(rot);

	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(rot);
	m_modelRender.Update();
}

void Seesaw::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}