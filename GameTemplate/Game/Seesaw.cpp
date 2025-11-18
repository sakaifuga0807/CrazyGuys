#include "stdafx.h"
#include "Seesaw.h"
#include "Player.h"
#include "JsonUtility.h"


bool Seesaw::Start()
{
	m_modelRender.Init("Assets/objectData/Seesaw.tkm");

	json configData;
	//Jsonファイルを読み込む。
	if (!JsonUtility::LoadJson("Assets/config/Seesaw.json", configData))
	{
		return false;
	}

	//Seesawの値を変数に格納。
	auto seesawData = configData["Seesaw"];

	//回転速度。
	m_angleSpeed = seesawData["AngleSpeed"];

	//制限角度。
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

//プレイヤーが乗っているかを確認する関数。
void Seesaw::UpdateSeesawState()
{
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

	//座標と回転を反映させる。
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