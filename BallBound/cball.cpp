#include <QPainter>
#include "cball.h"

float g = -9.8f;
float boxSize = 0;

void CBall::move(float dt)
{
	// 前回の結果を記録
	Vector2f vel0 = m_Vel;
	// 計算
	m_Vel.y = vel0.y + (g * dt);
	m_Pos = m_Pos + (vel0 + m_Vel) * dt / 2.0f;
}

void CBall::UpdateMove(float dt)
{
	// シミュレーションをリセットするために、初期状態を保存する
	m_baseVel = m_Vel;
	m_basePos = m_Pos;

	move(dt);

	/*// 移動したときの範囲をボックス上にする
	m_Rect.Empty();
	m_Rect.Expand(m_basePos, m_radius);
	m_Rect.Expand(m_Pos, m_radius);*/
}

void CBall::UpdateCollideBoll(float dt, CBall &other)
{
	float dis = m_Pos.GetDistance(other.m_Pos);
	if (m_Radius * 2 < dis)
	{
		return;
	}

	// 重心速度
	Vector2f Vc = ((m_Mass * m_Vel) + (other.m_Mass * other.m_Vel)) / (m_Mass + other.m_Mass);
	// 重心からの相対速度
	Vector2f Va = m_Vel - Vc;
	Vector2f Vb = other.m_Vel - Vc;
	// 互いの重心を結んだベクトル
	Vector2f N = m_Pos - other.m_Pos;
	Vector2f Ha = N * N.dot(Va) / N.dot(N);
	Va = Va - 2 * Ha;
	Vector2f Hb = N * N.dot(Vb) / N.dot(N);
	Vb = Vb - 2 * Hb;

	m_Vel = Va + Vc;
	other.m_Vel = Vb + Vc;

	m_Pos = m_basePos;
	other.m_Pos = other.m_basePos;
}

void CBall::UpdateCollideWall(float /*dt*/, float maxPos)
{
	// 接地判定
	if ((m_Pos.y - m_Radius) <= 0.0f)
	{
		m_Vel.y = -m_Vel.y;
		m_Pos.y = m_Radius;
	}

	// 壁との衝突判定
	if (maxPos <= (m_Pos.x + m_Radius))
	{
		m_Vel.x = -m_Vel.x;
		m_Pos.x = maxPos - m_Radius;
	}

	if ((m_Pos.x - m_Radius) <= -maxPos)
	{
		m_Vel.x = -m_Vel.x;
		m_Pos.x = -maxPos + m_Radius;
	}
}

void CBall::draw(QPainter &painter)
{
	// 単位変換
	int r = m_Radius * 1000;
	Vector2 position{ (int)(m_Pos.x * 1000), (int)(m_Pos.y * 1000) };
	// 描画
	painter.setPen(QPen(col, 2, Qt::SolidLine, Qt::FlatCap));
	painter.drawEllipse((position.x - r), (position.y - r), r * 2, r * 2);
	//painter.drawRect(m_Rect.minX * 1000, m_Rect.minY * 1000, (m_Rect.maxX - m_Rect.minX) * 1000, (m_Rect.maxY - m_Rect.minY) * 1000);
}

void CBall::setInitialValue(Vector2f initialPos, Vector2f speed)
{
	// 代入
	m_Pos = initialPos;
	m_Vel = speed;
}

void CBall::setBall(int r, Qt::GlobalColor color, bool a)
{
	m_Radius = (float)r / 1000;
	col = color;
	A = a;
}