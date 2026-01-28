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
#include "StageObjectFactory.h"
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
	//フェードイン演出を開始する。
	FadeManager::GetInstance()->StartFadeIn(0.5f);

	//乱数を初期化する。シーソーなどのランダム挙動用。
	srand(static_cast<unsigned int>(time(nullptr)));

	//背景オブジェクトを生成する。
	m_backGround = NewGO<BackGround>(0, "background");

	//ゴールオブジェクトを生成する。
	m_goal = NewGO<Goal>(1, "goal");

	//プレイヤーを生成する。
	MakePlayers();

	//ゲームカメラを生成する。最初はプレイヤー0を追従する。
	m_gameCamera = NewGO<GameCamera>(0, "gamecamera");
	m_gameCamera->SetTarget(m_players[0]);

	//カウントダウン管理オブジェクトを生成する。
	m_countdown = NewGO<CountdownManager>(1, "countdown");

	//ステージギミック生成用Factoryを生成する。
	//生成したオブジェクトの寿命管理もFactoryが担当する。
	m_stageFactory = NewGO<StageObjectFactory>(0, "stageFactory");

	//ゲーム全体設定をJSONから読み込む。
	GameConfig();

	//初期フェーズはカメラ演出。
	m_gamePhase = enGamePhase_CameraDemo;
	m_phaseTimer = 0.0f;

	//カウントダウン終了までプレイヤー操作を無効にする。
	for (auto player : m_players)
	{
		player->SetCanMove(false);
	}

	//ゲームの準備完了を通知する。
	m_isReady = true;

	return true;
}

Game::Game()
{

}

Game::~Game()
{
	//ステージギミックをまとめて破棄する。
	//個別のDeleteGOはStageObjectFactoryのデストラクタ内で行われる。
	DeleteGO(m_stageFactory);

	//背景を削除する。
	DeleteGO(m_backGround);

	//カメラを削除する。
	DeleteGO(m_gameCamera);

	//ゴールを削除する。
	DeleteGO(m_goal);

	//プレイヤーを全て削除する。
	for (auto player : m_players)
	{
		DeleteGO(player);
	}
	m_players.clear();

	//スカイキューブを削除する。
	DeleteGO(m_skyCube);
}


void Game::Update()
{
	//ゴール後はフェーズに関係なくリザルト遷移待ちを行う。
	if (m_isGoal)
	{
		m_goalTimer -= g_gameTime->GetFrameDeltaTime();

		if (m_goalTimer <= 0.0f)
		{
			GoToResult(m_winner);
			return;
		}
	}

	//ゲーム進行フェーズごとの処理。
	switch (m_gamePhase)
	{
	case enGamePhase_CameraDemo:
		//開始時のカメラ演出を更新する。
		UpdateCameraDemo();
		return;
	case enGamePhase_Countdown:
		//カウントダウン終了後にゲーム開始。
		if (m_countdown && m_countdown->IsFinished())
		{
			m_gamePhase = enGamePhase_Playing;
			DeleteGO(m_countdown);

			//プレイヤー操作を有効にする。
			for (auto player : m_players)
			{
				player->SetCanMove(true);
			}

			//BGMを再生する。
			SoundManager::Get().LoadFromJson("Assets/config/Sound.json");
			SoundManager::Get().Play("GameBGM");
		}
		return;

	case enGamePhase_Playing:
		//通常のゲーム進行。
		break;
	}

	//安全なタイミングでGameを削除する。
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
	//多重呼び出しを防止する。
	if (m_isDelete)
	{
		return;
	}
	m_isDelete = true;

	//BGMを停止する。
	SoundManager::Get().StopBGM();

	//カメラの更新を停止する。
	if (m_gameCamera)
	{
		m_gameCamera->m_enable = false;
	}

	//リザルト画面を生成する。
	auto result = NewGO<Result>(3, "result");

	//勝者がいる場合はモデルを設定する。
	if (winner)
	{
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