#pragma once

class Game;

class Load: public IGameObject
{
public:
	bool Start() override;
	Load();
	~Load();
	void Update();
	void Render(RenderContext& rc);

private:
	Game* m_game;
	SpriteRender m_sprite;
	SpriteRender m_spriteCharacter;
	
	float m_timer=0.0f;
	bool m_isFinish = false;
};