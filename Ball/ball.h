#pragma once

class QPainter;
namespace Qt
{
	enum GlobalColor;
};

class CBall
{
public:
	CBall()
	{
	}

	void Set(float timeMove, Qt::GlobalColor col)
	{
		m_moveDuration = timeMove;
		m_color = col;
	}

	// 1フレーム分ボールを移動する
	void Update(float dt);

	// ボールを描画する
	void Draw(QPainter &painter, int gridWidth, int gridSpan);

	float m_DurationTime = 0;
	int m_turning = 0;
	float m_moveDuration = 0;
	Qt::GlobalColor m_color;
};