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

void CBall::Update(float dt, float maxPos, CBall other[], int arraySize, int myNumber)
{
	// 細かさ
	int div = 16;
	//col = Qt::red;
	boxSize = maxPos;

	// シミュレーションをリセットするために、初期状態を保存する
	Vector2f base_vel = m_Vel;
	Vector2f base_pos = m_Pos;

	// 落下
	move(dt);

	// 接地判定
	if ((m_Pos.y - m_radius) <= 0.0f)
	{
		// 計算をやりなおす
		m_Vel = base_vel;
		m_Pos = base_pos;

		if (A == false)
		{
			// 接地するまで落下
			int countTime = 0;
			float div_dt = dt / (float)div;
			for (int i = 0; i < div; i++)
			{
				move(div_dt);
				if ((m_Pos.y - m_radius) <= 0.0f) break;
				countTime++;
			}
			m_Pos.y = m_radius;

			// 速度ベクトルの向きを反転する(適当な減衰率をかける)
			m_Vel.y = -m_Vel.y * 0.92f;

			// 残り時間のシミュレーションを行う
			float dt1 = dt * (div - countTime) / div;
			move(dt1);
		}
		else
		{
			// 接地する瞬間までの経過時間 dt0 を求め、そこまでのシミュレーションを行う
			float B = (m_Vel.y * m_Vel.y) - (2.0f * g * (m_Pos.y - m_radius));
			float dt0 = (-m_Vel.y - std::sqrt(B)) / g;
			move(dt0);

			// 速度ベクトルの向きを反転する(適当な減衰率をかける)
			m_Vel.y = -m_Vel.y * 0.92f;

			// 残り時間のシミュレーションを行う
			float dt1 = dt - dt0;
			move(dt1);
		}

		// 計算の結果、高さが床より下になるようなら、強制的に座標を戻す
		// 数値計算の誤差により、ボール座標が床の上に戻らない可能性がある
		if ((m_Pos.y - m_radius) <= 0.0f)
		{
			m_Pos.y = m_radius;
			m_Vel.y = 0.0f;
		}
	}

	// 移動したときの範囲をボックス上にする
	m_Rect.Empty();
	m_Rect.Expand(base_pos, m_radius);
	m_Rect.Expand(m_Pos, m_radius);

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
	
	// ほかのボールといちを比較
	for (int i = 0; i <= arraySize; i++)
	{
		// 自分のことは計算しない
		if (i != myNumber)
		{
			bool overlapped = m_Rect.isOverlapped(other[i].m_Rect);
			if (overlapped)
			{
				m_Pos = base_pos;
				m_Vel = base_vel;
				// 少しづつ動かして衝突するか確認
				int countTime = 0;
				float div_dt = dt / (float)div;
				for (int i = 0; i <= div; i++)
				{
					move(div_dt);
					float distance = m_Pos.GetDistance(other[i].m_Pos);
					countTime++;
					if (distance <= m_radius)
					{
						col = Qt::blue;
						break;
					}
				}

				// のこりを計算
				move(div_dt * (div - countTime));
			}
		}
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
	painter.drawRect(m_Rect.minX * 1000, m_Rect.minY * 1000, (m_Rect.maxX - m_Rect.minX) * 1000, (m_Rect.maxY - m_Rect.minY) * 1000);
}

void CBall::setInitialValue(Vector2f initialPos, Vector2f speed)
{
	// 代入
	m_Pos = initialPos;
	m_Vel = speed;
}

void CBall::setBall(int r, Qt::GlobalColor color, bool a)
{
	m_radius = (float)r / 1000;
	col = color;
	A = a;
}