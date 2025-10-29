#include "stdafx.h"
#include "Game.h"
#include "BackGround.h"
#include "Player.h"
#include "GameCamera.h"
#include "Goal.h"
#include "Hammer.h"
#include "Seesaw.h"
#include "RotationGround.h"
#include "nature/SkyCube.h"
#include "JsonUtility.h"
#include <cstdlib>
#include <ctime>

bool Game::Start()
{
	//乱数の初期化。
	srand(static_cast<unsigned int>(time(nullptr)));

	//ステージを生成。
	m_backGround=NewGO<BackGround>(0,"background");
	//カメラを生成。
	m_gameCamera = NewGO<GameCamera>(0, "gamecamera");
	//プレイヤーの生成。
	m_player = NewGO<Player>(1, "player");
	//ゴールの生成。
	m_goal = NewGO<Goal>(0, "goal");

	//回転床の生成。
	//Jsonデータを格納する。
	json configDataRotationGround;
	if (!JsonUtility::LoadJson("Assets/config/RotationGroundList.json", configDataRotationGround))
	{
		return false;
	}
	//パースを設定。
	auto RotationGroundArray = configDataRotationGround["RotationGrounds"];

	for (int i = 0; i < RotationGroundArray.size(); i++)
	{
		auto data= RotationGroundArray[i];
		//座標、回転、スケールを取得。
		auto pos = data["Position"];
		auto rot = data["Rotation"];
		auto scale = data["Scale"];

		Vector3 position(pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>());
		Quaternion rotation(rot[0].get<float>(), rot[1].get<float>(), rot[2].get<float>(), rot[3].get<float>());
		Vector3 scaling(scale[0].get<float>(), scale[1].get<float>(), scale[2].get<float>());
		//回転速度も設定する。
		float angleSpeed = data["AngleSpeed"].get<float>();

		//回転床を生成。
		auto rotationGround = NewGO<RotationGround>(0, ("rotationground" + std::to_string(i)).c_str());
		rotationGround->SetPosition(position);
		rotationGround->SetRotation(rotation);
		rotationGround->SetScale(scaling);
		rotationGround->SetAngleSpeed(angleSpeed);

		m_rotationGrounds.push_back(rotationGround);
	}


	//ハンマーの生成。
	//Jsonデータを格納する。
	json configDataHammer;
	//ファイルを読み込む。
	if (!JsonUtility::LoadJson("Assets/config/HammerList.json", configDataHammer))
	{
		return false;
	}

	//パースを設定。
	auto hammerArray = configDataHammer["Hammers"];

	for(int i=0;i<hammerArray.size();i++)
	{
		auto data = hammerArray[i];
		//座標、回転、スケールを取得。
		auto pos = data["Position"];
		auto rot = data["Rotation"];
		auto scale = data["Scale"];

		Vector3 position(pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>());
		Quaternion rotation(rot[0].get<float>(), rot[1].get<float>(), rot[2].get<float>(), rot[3].get<float>());
		Vector3 scaling(scale[0].get<float>(), scale[1].get<float>(), scale[2].get<float>());
		
		//ハンマーを生成。
		auto hammer = NewGO<Hammer>(0, ("hammer" + std::to_string(i)).c_str());
		hammer->SetPosition(position);
		hammer->SetRotation(rotation);
		hammer->SetScale(scaling);
		m_hammers.push_back(hammer);
	}


	//シーソーの生成。
	//Jsonデータを格納する。
	json configDataSeesaw;
	//ファイルを読み込む。
	if (!JsonUtility::LoadJson("Assets/config/SeesawList.json", configDataSeesaw))
	{
		return false;
	}

	//パースを設定。
	auto seesawArray = configDataSeesaw["Seesaws"];

	for (int i = 0; i < seesawArray.size(); i++)
	{
		auto data = seesawArray[i];

		//座標、回転、スケールを取得。
		auto pos = data["Position"];
		auto rot = data["Rotation"];
		auto scale = data["Scale"];

		Vector3 position(pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>());
		Quaternion rotation(rot[0].get<float>(), rot[1].get<float>(), rot[2].get<float>(), rot[3].get<float>());
		Vector3 scaling(scale[0].get<float>(), scale[1].get<float>(), scale[2].get<float>());

		//シーソーを生成。
		auto seesaw = NewGO<Seesaw>(0, ("seesaw" + std::to_string(i)).c_str());
		seesaw->SetPosition(position);
		seesaw->SetRotation(rotation);
		seesaw->SetScale(scaling);

		//ランダムでシーソーの動きを決定。
		if (rand() % 2 == 0)
		{
			seesaw->SetMovingState(Seesaw::enMovingSeesaw::enUp);
		}
		else
		{
			seesaw->SetMovingState(Seesaw::enMovingSeesaw::enDown);
		}

		m_seesaws.push_back(seesaw);
	}

	//ゲーム全体の設定を読み込む。
	json configData;
	if(!JsonUtility::LoadJson("Assets/config/Game.json", configData))
    {
        return false;
    }

	auto GameConfigData = configData["Game"];

	//スカイキューブの座標。
	auto skyCubePos = GameConfigData["SkyCubePosition"];
	m_skyCubePosition = Vector3(skyCubePos[0],skyCubePos[1],skyCubePos[2]);
	//スカイキューブの大きさ。
	m_skyCubeScale = GameConfigData["SkyCubeScale"];
	//スカイキューブの明るさ。
	m_skyCubeLuminance = GameConfigData["SkyCubeLuminance"];
	//画像の位置。
	auto spritePos = GameConfigData["SpritePosition"];
	m_spritePosition = Vector3(spritePos[0], spritePos[1], spritePos[2]);
	//文字の位置。
	auto fontPos = GameConfigData["FontPosition"];
	m_fontPosition = Vector3(fontPos[0], fontPos[1], fontPos[2]);
	
	//スカイキューブを作成
	m_skyCube = NewGO<SkyCube>(0, "skycube");
	m_skyCube->SetType(enSkyCubeType_DayToon);
	m_skyCube->SetPosition(m_skyCubePosition);
	m_skyCube->SetScale(m_skyCubeScale);
	m_skyCube->SetLuminance(m_skyCubeLuminance);

	//UI画像を初期化。
	m_spriteRender.Init("Assets/sprite/CrazyGuysUI.dds", 400.0f, 150.0f);
	m_spriteRender.SetPosition(m_spritePosition);
	m_spriteRender.Update();

	//UI文字を初期化。
	m_fontRender.SetText(L"鍵を取ろう！");
	m_fontRender.SetPosition(m_fontPosition);
	m_fontRender.SetColor(g_vec4White);

	return true;
}

Game::Game()
{

}

Game::~Game()
{
	DeleteGO(m_backGround);
	DeleteGO(m_player);
	DeleteGO(m_gameCamera);
	DeleteGO(m_goal);
	for (auto hammer : m_hammers)
	{
		DeleteGO(hammer);
	}
	m_hammers.clear();
	for (auto seesaw : m_seesaws)
	{
		DeleteGO(seesaw);
	}
	m_seesaws.clear();
	DeleteGO(m_skyCube);
}

void Game::Update()
{
	if (m_isDelete)
	{
		DeleteGO(this);
	}
}

void Game::Render(RenderContext& rc)
{
	m_spriteRender.Draw(rc);
	m_fontRender.Draw(rc);
}