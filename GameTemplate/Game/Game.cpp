#include "stdafx.h"
#include "Game.h"
#include "BackGround.h"
#include "Player.h"
#include "GameCamera.h"
#include "Goal.h"
#include "Hammer.h"
#include "Seesaw.h"
#include "RotationGround.h"
#include "CountdownManager.h"
#include "GameSettings.h"
#include "nature/SkyCube.h"
#include "JsonUtility.h"
#include "Result.h"
#include "SoundManager.h"
#include <cstdlib>
#include <ctime>

bool Game::Start()
{
	//乱数の初期化。
	srand(static_cast<unsigned int>(time(nullptr)));

	//ステージを生成。
	m_backGround=NewGO<BackGround>(0,"background");

	//ゴールの生成。
	m_goal = NewGO<Goal>(0, "goal");

	//プレイヤーの生成。
	MakePlayers();

	//カメラの生成。
	m_gameCamera = NewGO<GameCamera>(0, "gamecamera");
	m_gameCamera->SetTarget(m_players[0]);

	//カウントダウンを生成。
	m_countdown = NewGO<CountdownManager>(0, "countdown");

	//回転床の生成。
	MakeRotationGrounds();
	
	//ハンマーの生成。
	MakeHammers();
	
	//シーソーの生成。
	MakeSeesaw();

	//ゲーム全体の設定。
	GameConfig();

	////////////使わなくなったら消してねーーーーーー///////////////
	m_debugFont.SetPosition(Vector3(250.0f, 400.0f, 0.0f));
	m_debugFont.SetScale(1.0f);
	m_debugFont.SetColor(g_vec4White);

	return true;
}

Game::Game()
{

}

Game::~Game()
{
	DeleteGO(m_backGround);
	DeleteGO(m_gameCamera);
	DeleteGO(m_goal);

	//プレイヤーを削除。
	for (auto player : m_players)
	{
		DeleteGO(player);
	}
	m_players.clear();

	//ハンマーを削除。
	for (auto hammer : m_hammers)
	{
		DeleteGO(hammer);
	}
	m_hammers.clear();

	//シーソーを削除。
	for (auto seesaw : m_seesaws)
	{
		DeleteGO(seesaw);
	}
	m_seesaws.clear();

	//回転する地面を削除。
	for (auto rotationGround : m_rotationGrounds)
	{
		DeleteGO(rotationGround);
	}
	m_rotationGrounds.clear();

	DeleteGO(m_skyCube);
}

void Game::Update()
{
	//カウントダウンが終了したか。
	if (!m_isCountdownFinished)
	{
		if (m_countdown&&m_countdown->IsFinished())
		{
			m_isCountdownFinished = true;
			DeleteGO(m_countdown);
			for (auto player : m_players)
			{
				//カウントダウン終了したことを伝える。
				player->SetCanMove(true);
				SoundManager::Get().LoadFromJson("Assets/config/Sound.json");
				SoundManager::Get().Play("GameBGM");
			}
		}
	}

	if (m_isDelete)
	{
		DeleteGO(this);
	}

	if (!m_players.empty())
	{
		Vector3 pos = m_players[0]->GetPosition();
		wchar_t buf[128];
		swprintf_s(buf, L"Pos: %.1f,%.1f,%.1f", pos.x, pos.y, pos.z);
		m_debugFont.SetText(buf);
	}

	if (m_isGoal)
	{
		m_goalTimer -= g_gameTime->GetFrameDeltaTime();

		if (m_goalTimer <= 0.0f)
		{
			NewGO<Result>(0, "result");
			DeleteGO(this);
			return;
		}
	}
}


//プレイヤーの生成処理。
void Game::MakePlayers()
{
	//プレイヤーの生成。
	int humanPlayerCount = GameSettings::PlayerCount;

	//一人プレイならAIを追加。
	bool isSinglePlayer = (humanPlayerCount == 1);

	//総プレイヤー数。
	int totalPlayers = isSinglePlayer ? 4 : humanPlayerCount;

	for (int i = 0; i < totalPlayers; i++)
	{
		//プレイヤーを生成。
		auto player = NewGO<Player>(0, ("Player" + std::to_string(i)).c_str());

		if (i < humanPlayerCount)
		{
			//一人プレイ。
			player->SetControllerIndex(i);
			player->SetIsAI(false);
		}
		else
		{
			//複数プレイ時。
			//コントローラーの番号を設定。
			player->SetControllerIndex(-1);
			player->SetIsAI(true);
		}

		//座標を少しずらして生成。
		player->SetPosition(Vector3(-400.0f + (i * 200.0f), 0.0f, 100.0f));
		m_players.push_back(player);
	}

	for (auto player : m_players)
	{
		player->SetGame(this);
	}
}

//回転床の生成処理。
void Game::MakeRotationGrounds()
{
	//回転床の生成。
	json configDataRotationGround;
	if (!JsonUtility::LoadJson("Assets/config/RotationGroundList.json", configDataRotationGround))
	{
		return;
	}

	//ノードを取得。
	auto RotationGroundArray = configDataRotationGround["RotationGrounds"];

	for (int i = 0; i < RotationGroundArray.size(); i++)
	{
		auto data = RotationGroundArray[i];
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
}

//ハンマーの生成処理。
void Game::MakeHammers()
{
	//ハンマーの生成。
	//Jsonデータを格納する。
	json configDataHammer;
	//ファイルを読み込む。
	if (!JsonUtility::LoadJson("Assets/config/HammerList.json", configDataHammer))
	{
		return;
	}

	//ノードを取得。
	auto hammerArray = configDataHammer["Hammers"];

	for (int i = 0; i < hammerArray.size(); i++)
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
}

//シーソーの生成処理。
void Game::MakeSeesaw()
{
	//シーソーの生成。
	//Jsonデータを格納する。
	json configDataSeesaw;
	//ファイルを読み込む。
	if (!JsonUtility::LoadJson("Assets/config/SeesawList.json", configDataSeesaw))
	{
		return;
	}

	//ノードを取得。
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
}

void Game::GameConfig()
{
	//ゲーム全体の設定を読み込む。
	json configData;
	if (!JsonUtility::LoadJson("Assets/config/Game.json", configData))
	{
		return;
	}

	//ノードを取得。
	auto GameConfigData = configData["Game"];

	//スカイキューブの座標。
	auto skyCubePos = GameConfigData["SkyCubePosition"];
	m_skyCubePosition = Vector3(skyCubePos[0], skyCubePos[1], skyCubePos[2]);
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

	//初期化。
	m_roundOverSprite.Init("Assets/sprite/RoundOver.dds", 1920.0f, 1080.0f);
	m_roundOverSprite.SetPosition(Vector3{ 0.0f,0.0f,0.0f });
	m_roundOverSprite.Update();

	//UI文字を初期化。
	m_fontRender.SetText(L"鍵を取ろう！");
	m_fontRender.SetPosition(m_fontPosition);
	m_fontRender.SetColor(g_vec4White);
}

void Game::ShowRoundOver()
{
	m_showRoundOver = true;
	SoundManager::Get().StopBGM();
}

void Game::OnPlayerGoal()
{
	if (m_isGoal)
	{
		return;
	}

	//フラグを立てる。
	m_isGoal = true;
	//待つ。
	m_goalTimer = 3.0f;
	//表示をする。
	ShowRoundOver();

	//プレイヤーを動かさないようにする。
	for (auto player : m_players)
	{
		player->SetCanMove(false);
	}
}

void Game::Render(RenderContext& rc)
{
	m_spriteRender.Draw(rc);
	m_fontRender.Draw(rc);
	m_debugFont.Draw(rc);
	if (m_showRoundOver)
	{
		m_roundOverSprite.Draw(rc);
	}
}