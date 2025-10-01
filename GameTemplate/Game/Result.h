#pragma once

class Title;

class Result:public IGameObject
{
public:
	bool Start();
	Result();
	~Result();
	void Update();
	void Render(RenderContext& rc);

private:
	//メンバ変数。
	Title* m_title;//タイトル。
	SpriteRender m_spriteRender;//スプライトレンダー。
};

