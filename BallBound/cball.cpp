#include <QPainter>
#include "cball.h"
#include "CubicEquation.h"
#include <windows.h>

float g = -9.8f;
float boxSize = 0;
float CBall::m_ReflectionCoef = 0.92f;

//-----------------------------------------------------------------------------
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

void CBall::UpdateCollideBoll(float /*dt*/, CBall &other)
{
	// 衝突がなければ何もしない
	float dis = m_Pos.GetDistance(other.m_Pos);
	if ((m_Radius + other.m_Radius) < dis)
	{
		return;
	}
	
	// 万が一、離れる方向なのに衝突が判定されていたら、衝突がなかったことにする
	{
		Vector2f V = other.m_Vel - m_Vel;
		Vector2f D = other.m_Pos - m_Pos;
		if (0.0f <= V.dot(D)) return;
	}

	#ifdef _DEBUG
	{
		float V0 = m_Mass * m_Vel.GetLength();
		float V1 = other.m_Mass * other.m_Vel.GetLength();
		char buffer[256];
		sprintf_s(buffer, "Before: V0 = %.3f V1 = %.3f V0 + V1 = %.3f\n", V0, V1, V0 + V1);
		OutputDebugStringA(buffer);
	}
	#endif

	// 重心を原点とした座標系で、反射計算を行う。
	// そうすると、お互いの速度ベクトルが、反対向きの並行になるので、シンプルに計算できる。
	// 重心からの相対速度
	Vector2f Vc = ((m_Mass * m_Vel) + (other.m_Mass * other.m_Vel)) / (m_Mass + other.m_Mass);
	Vector2f Va = m_Vel - Vc;
	Vector2f Vb = other.m_Vel - Vc;

	// 反射計算
	Vector2f N = (other.m_Pos - m_Pos).normalize();
	Va = N.reflect(Va) * m_ReflectionCoef;
	Vb = N.reflect(Vb) * m_ReflectionCoef;
	// 重心座標系から、通常の座標系に戻す
	m_Vel = (Va + Vc);
	other.m_Vel = (Vb + Vc);
	// 位置を、衝突した瞬間に移動(近似)
	Vector2f ct = m_Pos + (other.m_Pos - m_Pos) * m_Radius / (m_Radius + other.m_Radius);
	m_Pos = ct - N * m_Radius;
	other.m_Pos = ct + N * other.m_Radius;

	#ifdef _DEBUG
	{
		float V0 = m_Mass * m_Vel.GetLength();
		float V1 = other.m_Mass * other.m_Vel.GetLength();
		char buffer[256];
		sprintf_s(buffer, "After: V0 = %.3f V1 = %.3f V0 + V1 = %.3f\n", V0, V1, V0 + V1);
		OutputDebugStringA(buffer);
	}
	#endif
}

void CBall::UpdateCollideWall(float /*dt*/, float maxPos, float ParabolaFactor)
{
	// 接地判定
	/*if ((m_Pos.y - m_Radius) <= 0.0f)
	{
		m_Vel.y = -m_Vel.y * m_CoefReflection;
		m_Pos.y = m_Radius;
	}*/

	// ボールと放物線の交差判定
	// ボールに最も近い放物線(y = pf * x^2)上の座標pを求めるために、
	// (ボール中心 - p)と、pの接線の内積が0となることを表す式を作り、
	// 三次方程式を解く。虚数を持たない解が一つ得られる。
	// A*x^3+B*x+C
	const float pf = ParabolaFactor;
	double c3 = -2.0f * pf * pf;
	double c2 = 0.0f;
	double c1 = (2.0f * pf * m_Pos.y) - 1.0f;
	double c0 = m_Pos.x;
	double xR[3], xI[3];
	cubicEquation(c3, c2, c1, c0, xR, xI);
	// ボールに最も近い放物線上の座標pを求める
	Vector2f p{ (float)xR[0], (float)(pf * xR[0] * xR[0]) };
	float minDist = (p - m_Pos).GetLength();
	for (int i = 1; i < 3; i++)
	{
		if (abs(xI[i]) < 0.000001)
		{
			Vector2f pp{ (float)xR[i], (float)(pf * xR[i] * xR[i]) };
			float d = (pp - m_Pos).GetLength();
			if (d < minDist)
			{
				p = pp;
				minDist = d;
			}
		}
	}

//	char str[256];
	//sprintf(str, "x0 = %.3f + %.3fi\n", xR[0], xI[0]);
	//OutputDebugStringA(str);
	//sprintf(str, "x1 = %.3f + %.3fi\n", xR[1], xI[1]);
	//OutputDebugStringA(str);
	//sprintf(str, "x2 = %.3f + %.3fi\n", xR[2], xI[2]);
	//OutputDebugStringA(str);
	
	float floorDistance = m_Pos.GetDistance(p);
	if (floorDistance <= m_Radius)
	{
		// 接線T
		// Note: 放物線を示す二次式(y = pf * x^2)の微分(y = 2 * pf * x)
		Vector2f T{ 1.0f, (2.0f * pf * p.x) };

		// 法線N
		Vector2f N = Vector2f{ -T.y, T.x }.normalize();
		// とりあえず、上向きの方向にそろえておく
		if (N.y < 0.0f) N = -1.0f * N;

		// 速度ベクトル(m_Vel)を反射する
		// 反射係数をかける
		m_Vel = N.reflect(m_Vel) * m_ReflectionCoef;
		// ボールの位置を衝突の瞬間へ戻す
		m_Pos = p + m_Radius * N;
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

void CBall::setBall(int r, Qt::GlobalColor color, float mass)
{
	m_Radius = (float)r / 1000;
	col = color;
	m_Mass = mass;
}