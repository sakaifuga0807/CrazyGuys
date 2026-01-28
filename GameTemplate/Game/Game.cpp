#include "stdafx.h"
#include "Game.h"
#include "BackGround.h"
#include "Player.h"
#include "GameCamera.h"
#include "Goal.h"
#include "Hammer.h"
#include "Seesaw.h"
#include "RotationGround.h"
#include "Axe.h"
#include "CountdownManager.h"
#include "GameSettings.h"
#include "nature/SkyCube.h"
#include "JsonUtility.h"
#include "Result.h"
#include "SoundManager.h"
#include "FadeManager.h"
#include "MultiViewRender.h"
#include <cstdlib>
#include <ctime>

namespace
{
	const int MAX_PLAYERS = 4;//プレイヤーの最大数。
}

bool Game::m_isReady = false;

bool Game::Start()
{
	//フェードインする。
	FadeManager::GetInstance()->StartFadeIn(0.5f);

	//乱数の初期化。
	srand(static_cast<unsigned int>(time(nullptr)));

	//ステージを生成。
	m_backGround = NewGO<BackGround>(0, "background");

	//ゴールの生成。
	m_goal = NewGO<Goal>(1, "goal");

	//プレイヤーの生成。
	MakePlayers();

	m_multiviewRender = NewGO<MultiViewRender>(0, "multiView");

	//カメラの生成。
	m_gameCamera = NewGO<GameCamera>(0, "gamecamera");
	m_gameCamera->SetTarget(m_players[0]);

	//カウントダウンを生成。
	m_countdown = NewGO<CountdownManager>(1, "countdown");

	//回転床の生成。
	MakeRotationGrounds();

	//ハンマーの生成。
	MakeHammers();

	//シーソーの生成。
	MakeSeesaw();

	//斧の生成。
	MakeAxes();

	//ゲーム全体の設定。
	GameConfig();

	m_gamePhase = enGamePhase_CameraDemo;
	m_phaseTimer = 0.0f;

	for (auto player : m_players)
	{
		player->SetCanMove(false);
	}

	//準備完了を伝える。
	m_isReady = true;

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

	//斧を削除。
	for (auto axe : m_axes)
	{
		DeleteGO(axe);
	}
	m_axes.clear();

	DeleteGO(m_skyCube);
}

void Game::Update()
{
	//ゴール後のリザルト遷移は常に処理を行う。
	if (m_isGoal)
	{
		m_goalTimer -= g_gameTime->GetFrameDeltaTime();

		if (m_goalTimer <= 0.0f)
		{
			GoToResult(m_winner);
			return;
		}
	}

	//フェーズ別の処理。
	switch (m_gamePhase)
	{
	case enGamePhase_CameraDemo:
		UpdateCameraDemo();
		return;
	case enGamePhase_Countdown:
		if (m_countdown && m_countdown->IsFinished())
		{
			m_gamePhase = enGamePhase_Playing;
			DeleteGO(m_countdown);


			for (auto player : m_players)
			{
				player->SetCanMove(true);
			}

			SoundManager::Get().LoadFromJson("Assets/config/Sound.json");
			SoundManager::Get().Play("GameBGM");
		}
		return;

	case enGamePhase_Playing:
		break;
	default:
		break;
	}

	//その他の終了処理。
	if (m_isDelete)
	{
		DeleteGO(this);
		return;
	}
}


//プレイヤーの生成処理。
void Game::MakePlayers()
{
	//プレイヤーの生成。
	int humanPlayerCount = 0;

	const char* modelPaths[] =
	{
		"Assets/modelData/BlueGuys.tkm",
		"Assets/modelData/PinkGuys.tkm",
		"Assets/modelData/WhiteGuys.tkm",
		"Assets/modelData/Penguin.tkm"
	};

	std::vector<int> connectedPads;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (g_pad[i] && g_pad[i]->IsConnected())
		{
			connectedPads.push_back(i);
		}
	}

	//コントローラーが一つもつながれていなければ一人だけプレイヤーにする。
	if (connectedPads.size() == 0)
	{
		humanPlayerCount = 1;
	}
	else
	{
		humanPlayerCount = static_cast<int>(connectedPads.size());
		if (humanPlayerCount > MAX_PLAYERS)
		{
			humanPlayerCount = MAX_PLAYERS;
		}
	}

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		//プレイヤーを生成。
		auto player = NewGO<Player>(0, ("Player" + std::to_string(i)).c_str());

		player->SetModelPath(modelPaths[i % 4]);

		if (i == 0)
		{
			if (!connectedPads.empty())
			{
				player->SetControllerIndex(connectedPads[0]);
			}
			else
			{
				player->SetControllerIndex(0);
			}
		}
		else if (i < connectedPads.size())
		{
			player->SetControllerIndex(connectedPads[i]);
		}
		else
		{
			player->SetControllerIndex(-1);
		}

		//座標を少しずらして生成。
		player->SetPosition(Vector3(-400.0f + (i * 200.0f), 0.0f, 100.0f));
		player->SetGame(this);

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
		auto rotationGround = NewGO<RotationGround>(1, ("rotationground" + std::to_string(i)).c_str());
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
		auto hammer = NewGO<Hammer>(1, ("hammer" + std::to_string(i)).c_str());
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
		auto seesaw = NewGO<Seesaw>(1, ("seesaw" + std::to_string(i)).c_str());
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

void Game::MakeAxes()
{
	//斧の生成。
	//Jsonデータを格納する。
	json configDataAxes;
	//ファイルを読み込む。
	if (!JsonUtility::LoadJson("Assets/config/AxeList.json", configDataAxes))
	{
		return;
	}

	//ノードを取得。
	auto axeArray = configDataAxes["Axes"];

	for (int i = 0; i < axeArray.size(); i++)
	{
		auto data = axeArray[i];

		//座標、回転、スケールを取得。
		auto pos = data["Position"];
		auto rot = data["Rotation"];
		auto scale = data["Scale"];
		Vector3 position(pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>());
		Quaternion rotation(rot[0].get<float>(), rot[1].get<float>(), rot[2].get<float>(), rot[3].get<float>());
		Vector3 scaling(scale[0].get<float>(), scale[1].get<float>(), scale[2].get<float>());

		//速度と範囲を取得。
		float speed = data["Speed"];
		float range = data["Range"];

		//斧を生成。
		auto axe = NewGO<Axe>(2, ("axe" + std::to_string(i)).c_str());
		axe->SetPosition(position);
		axe->SetRotation(rotation);
		axe->SetScale(scaling);
		axe->SetSpeed(speed);
		axe->SetRange(range);

		m_axes.push_back(axe);
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
	m_skyCube = NewGO<nsK2Engine::SkyCube>(3, "skycube");
	m_skyCube->SetType(enSkyCubeType_DayToon);
	m_skyCube->SetPosition(m_skyCubePosition);
	m_skyCube->SetScale(m_skyCubeScale);
	m_skyCube->SetLuminance(m_skyCubeLuminance);

	//UI画像を初期化。
	m_spriteRender.Init("Assets/sprite/CrazyGuysUI.dds", 500.0f, 150.0f);
	m_spriteRender.SetPosition(m_spritePosition);
	m_spriteRender.Update();

	//初期化。
	m_roundOverSprite.Init("Assets/sprite/RoundOver.dds", 1920.0f, 1080.0f);
	m_roundOverSprite.SetPosition(Vector3{ 0.0f,0.0f,0.0f });
	m_roundOverSprite.Update();

	//UI文字を初期化。
	m_fontRender.SetText(L"ゴールを目指そう！");
	m_fontRender.SetPosition(m_fontPosition);
	m_fontRender.SetColor(g_vec4White);
}

void Game::ShowRoundOver()
{
	if (m_showRoundOver)
	{
		return;
	}

	m_showRoundOver = true;
	SoundManager::Get().StopBGM();
}

void Game::OnPlayerGoal(Player* winner)
{
	//すでにゴールしたら無視。
	if (m_isGoal)
	{
		return;
	}

	//勝者を設定。
	m_winner = winner;

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

void Game::GoToResult(Player* winner)
{
	if (m_isDelete)
	{
		return;
	}
	m_isDelete = true;

	//BGM停止。
	SoundManager::Get().StopBGM();

	//カメラ無効化。
	if (m_gameCamera)
	{
		m_gameCamera->m_enable = false;
	}

	//リザルト生成。
	auto result = NewGO<Result>(3, "result");

	if (winner)
	{
		//モデルパスを取得。
		result->SetWinnerModelPath(winner->GetModelPath());
	}

	DeleteGO(this);
}

void Game::UpdateCameraDemo()
{
	//カメラがイージングし終わったらフェードアウト開始。
	if (!m_isFadeOut && !m_isFadeIn)
	{
		if (m_gameCamera->IsCameraDemoFinished())
		{
			m_isFadeOut = true;
			FadeManager::GetInstance()->StartFadeOut(0.5f);
		}
		return;
	}

	//フェードアウト完了したらフェードイン開始。
	if (m_isFadeOut)
	{
		if (!FadeManager::GetInstance()->IsFadeing())
		{
			m_isFadeOut = false;
			m_isFadeIn = true;

			FadeManager::GetInstance()->StartFadeIn(0.5f);
		}
		return;
	}

	//フェードイン完了したらカウントダウンへ。
	if (m_isFadeIn)
	{
		if (!FadeManager::GetInstance()->IsFadeing())
		{
			m_isFadeIn = false;

			//カウントダウンへ変える。
			m_gamePhase = enGamePhase_Countdown;

			//カメラを追従状態へ切り替え。
			m_gameCamera->StartFollow();

			//カウントダウンを開始。
			if (m_countdown)
			{
				m_countdown->StartCountdown();
			}
		}
		return;
	}
}

void Game::Render(RenderContext& rc)
{
	m_spriteRender.Draw(rc);
	m_fontRender.Draw(rc);
	if (m_showRoundOver)
	{
		m_roundOverSprite.Draw(rc);
	}
}