#include <QPainter>
#include "cball.h"

float g = -9.8f;
float boxSize = 0;

void CBall::UpDate(float dt, float maxPos)
{
	boxSize = maxPos;
	Vector2f pos0 = m_Pos;
	Vector2f vel0 = m_Vel;

	// 落下
	m_Vel.y = vel0.y + (g * dt);
	m_Pos.y = pos0.y + (vel0.y + m_Vel.y) * dt / 2.0f;
	m_Pos.x = pos0.x + (vel0.x + m_Vel.x) * dt / 2.0f;

	// 接地判定
	if ((m_Pos.y - m_radius) <= 0.0f)
	{
		// 接地する瞬間までの経過時間 dt0 を求め、そこまでのシミュレーションを行う
		float A = (vel0.y * vel0.y) - (2.0f * g * (pos0.y - m_radius));
		float dt0 = (-vel0.y - std::sqrt(A)) / g;
		m_Vel.y = vel0.y + (g * dt0);
//		m_Pos.y = pos0.y + (vel0.y + m_Vel.y) * dt0 / 2.0f;
		m_Pos.y = m_radius;
		m_Pos.x = pos0.x + (vel0.x + m_Vel.x) * dt0 / 2.0f;

		// 速度ベクトルの向きを反転する(適当な減衰率をかける)
		m_Vel.y = -0.95f * m_Vel.y;
		vel0.y = m_Vel.y;

		// 残り時間のシミュレーションを行う
		float dt1 = dt - dt0;
		m_Vel.y = m_Vel.y + (g * dt1);
		m_Pos.y = m_Pos.y + (vel0.y + m_Vel.y) * dt1 / 2.0f;
		m_Pos.x = m_Pos.x + (vel0.x + m_Vel.x) * dt1 / 2.0f;

		// 計算の結果、高さが床より下になるようなら、強制的に座標を戻す
		// 数値計算の誤差により、ボール座標が床の上に戻らない可能性がある
		if ((m_Pos.y - m_radius) <= 0.0f)
		{
			m_Pos.y = m_radius;
			m_Vel.y = 0.0f;
		}
	}

	if (boxSize < (m_Pos.x + m_radius))
	{
		m_Vel.x = -m_Vel.x;
		m_Pos.x = boxSize - m_radius;
	}
	
	if ((m_Pos.x - m_radius) < -boxSize)
	{
		m_Vel.x = -m_Vel.x;
		m_Pos.x = -boxSize + m_radius;
	}
}

void CBall::draw(QPainter &painter)
{
	// 単位変換
	int r = m_radius * 1000;
	Vector2 position{ (int)(m_Pos.x * 1000), (int)(m_Pos.y * 1000) };
	// 描画
	painter.setPen(QPen(col, 2, Qt::SolidLine, Qt::FlatCap));
	painter.drawEllipse((position.x - r), (position.y - r), r * 2, r * 2);
}

void CBall::setInitialValue(Vector2f initialPos, Vector2f speed)
{
	// 代入
	m_Pos = initialPos;
	m_Vel = speed;
}

void CBall::setBall(int r, Qt::GlobalColor color)
{
	m_radius = (float)r / 1000;
	col = color;
}