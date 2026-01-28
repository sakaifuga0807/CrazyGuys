#pragma once

class Player;
class GameCamera;


class MultiViewRender:public IGameObject
{
public:
	static const int MAX_PLAYER = 4;

	MultiViewRender() = default;
	~MultiViewRender() = default;

	bool Start();
	//Playerの情報を受け取る。
	void SetPlayers(const std::vector<Player*>& players)
	{
		m_players = players;
	}
	//レンダーターゲットを初期化。
	void InitRenderTargets();
	//カメラを初期化。
	void InitCameras();
	//プレイヤーモデルを初期化。
	void InitPlaneModels();
	void Render(RenderContext& rc);

private:
	int m_playerCount = 0;

	RenderTarget m_renderTargets[MAX_PLAYER];
	Model        m_planeModels[MAX_PLAYER];

	Camera       m_drawHumanModelCamera;
	StructuredBuffer m_worldMatrixSB;

	std::vector<Player*> m_players;
};