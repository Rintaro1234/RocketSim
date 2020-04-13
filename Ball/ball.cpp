#include "variable.h"
#include "ball.h"

#include <QtWidgets>

// 1フレーム分ボールを移動する
void CBall::Update(float dt)
{
	m_DurationTime += dt;

	// 今の移動辺の終点に到達したとき、方向を変える
	if (m_moveDuration <= m_DurationTime)
	{
		m_turning++;
		m_DurationTime -= m_moveDuration;

		// 一周周回したときのリセット
		if (3 < m_turning) m_turning = 0;
	}
}

// ボールを描画する
void CBall::Draw(QPainter &painter, int gridWidth, int gridSpan)
{
	/* 円 */

	// 移動経路の角の位置(0と4は同じ座標)
	Vector2 absCornerPos = {
		gridWidth - gridSpan / 2,
		gridWidth - gridSpan / 2
	};

	cornerPos[4] = cornerPos[0];

	// 移動座標の計算
	Vector2 p0 = cornerPos[m_turning];
	Vector2 p1 = cornerPos[m_turning + 1];
	Vector2 dt = {
		p1.x - p0.x,
		p1.y - p0.y,
	};
	Vector2 Pos;
	Pos.x = (int)(p0.x + (dt.x * m_DurationTime) / m_moveDuration);
	Pos.y = (int)(p0.y + (dt.y * m_DurationTime) / m_moveDuration);

	// 移動する円の描画
	int Radius = gridSpan * 0.8 / 2;
	Vector2 LeftTop = {
		Pos.x - Radius,
		Pos.y - Radius,
	};
	painter.setPen(QPen(m_color, 2, Qt::SolidLine, Qt::RoundCap));
	painter.drawEllipse(LeftTop.x, LeftTop.y, 2 * Radius, 2 * Radius);
}
