#include "pch.h"
#include <QPainter>
#include "cball.h"
#include "CubicEquation.h"

using namespace std;

FLOAT_T g = -9.8f;
FLOAT_T boxSize = 0;

CBallPos *CBall::sm_posDataBuf = nullptr;
FLOAT_T CBall::sm_ReflectionCoef = 0.92f;
Vector2f CBall::sm_Ft;

//-----------------------------------------------------------------------------
void CBall::move(FLOAT_T dt)
{
	CBallPos &posData = sm_posDataBuf[m_index];

	// 前回の結果を記録
	Vector2f vel0 = m_Vel;
	// 計算
	m_Vel.y = vel0.y + (g * dt);
	posData.m_Pos = posData.m_Pos + (vel0 + m_Vel) * dt / 2.0f;
}

void CBall::UpdateMove(spaceGrid *grid, FLOAT_T dt)
{
	CBallPos &posData = sm_posDataBuf[m_index];

	// シミュレーションをリセットするために、初期状態を保存する
	m_baseVel = m_Vel;
	m_basePos = posData.m_Pos;

	// 移動計算
	move(dt);

	// 各Cellにわける
	FLOAT_T xx = posData.m_Pos.x - grid->minX;
	FLOAT_T lightSide = xx - posData.m_Radius;
	FLOAT_T rightSide = xx + posData.m_Radius;
	//	grid->mulIdxInvWidth = grid->numCells / (grid->maxX - grid->minX);
	int Ridx = floor(rightSide * grid->mulIdxInvWidth);
	int Lidx = floor(lightSide * grid->mulIdxInvWidth);
	if (Lidx < 0) Lidx = 0;
	const int N = grid->numCells - 1;
	if (N < Ridx) Ridx = N;
	for (int i = Lidx; i <= Ridx; i++)
	{
		// ボールが入っている Cell に、ボールを登録する
		lineComponent &X = grid->components[i];
		X.Y_Idx[X.numComponent].Idx = m_index;
		X.Y_Idx[X.numComponent].y = posData.m_Pos.y + posData.m_Radius;
		X.numComponent++;
	}
}

// ボール間の隙間を求める
float CBallPos::GetInterspace(const CBallPos &ball) const
{
	__m128 a = _mm_loadu_ps(&m_Pos.x);			// a = { b0.x, b0.y, b0.r, - }
	__m128 b = _mm_loadu_ps(&ball.m_Pos.x);		// b = { b1.x, b1.y, b1.r, - }
	__m128 c = _mm_sub_ps(a, b);				// c = { b0.x-b1.x, b0.y-b1.y, -, - }
	__m128 d = _mm_mul_ps(c, c);				// d = { (b0.x-b1.x)^2, (b0.y-b1.y)^2, -, - }
	__m128 e = _mm_shuffle_ps(d, d, _MM_SHUFFLE(0, 1, 0, 1));	// d = { (b0.y-b1.y)^2, (b0.x-b1.x)^2, -, - }
	__m128 f = _mm_add_ps(d, e);				// f = { (b0.x-b1.x)^2 + (b0.y-b1.y)^2, ... }
//	__m128 g = _mm_sqrt_ss(f);					// g = { SQRT((b0.x-b1.x)^2 + (b0.y-b1.y)^2), ... }
	__m128 h = _mm_add_ps(a, b);				// h = { -, -, b0.r+b1.r. - }
	__m128 i = _mm_shuffle_ps(h, h, _MM_SHUFFLE(2, 2, 2, 2));	// i = { b0.r+b1.r, -, -, - }
	__m128 j = _mm_mul_ss(i, i);				// j = { (b0.r+b1.r)^2, -, -, - }
	__m128 k = _mm_sub_ss(f, j);				// j = { ((b0.x-b1.x)^2 + (b0.y-b1.y)^2) - (b0.r+b1.r)^2, -, -, - }

	float dis;
	_mm_store_ss(&dis, k);
	return dis;
}

void CBall::UpdateCollideBall(FLOAT_T /*dt*/, CBall &other)
{
	// 本関数の外で衝突判定を行い、衝突のあるペアのみ処理する。
#if 0
	// 衝突がなければ何もしない
#if 0
	FLOAT_T dis = m_Pos.GetDistance(other.m_Pos);
	if ((m_Radius + other.m_Radius) < dis)
	{
		return;
	}
#else
	if (0.0f < m_posData->GetInterspace(*other.m_posData))
	{
		return;
	}
#endif
#endif

	CBallPos &posData = sm_posDataBuf[m_index];
	CBallPos &otherPosData = sm_posDataBuf[other.m_index];

	// 万が一、離れる方向なのに衝突が判定されていたら、衝突がなかったことにする
	{
		Vector2f V = other.m_Vel - m_Vel;
		Vector2f D = otherPosData.m_Pos - posData.m_Pos;
		if (0.0f <= V.dot(D)) return;
	}

	#ifdef _DEBUG
	{
		FLOAT_T V0 = m_Mass * m_Vel.GetLength();
		FLOAT_T V1 = other.m_Mass * other.m_Vel.GetLength();
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
	Vector2f N = (otherPosData.m_Pos - posData.m_Pos).normalize();
	Va = N.reflect(Va) * sm_ReflectionCoef;
	Vb = N.reflect(Vb) * sm_ReflectionCoef;
	// 重心座標系から、通常の座標系に戻す
	m_Vel = (Va + Vc);
	other.m_Vel = (Vb + Vc);
	// 位置を、衝突した瞬間に移動(近似)
	Vector2f ct = posData.m_Pos + (otherPosData.m_Pos - posData.m_Pos) * posData.m_Radius / (posData.m_Radius + otherPosData.m_Radius);
	posData.m_Pos		= ct - N * posData.m_Radius;
	otherPosData.m_Pos  = ct + N * otherPosData.m_Radius;

	#ifdef _DEBUG
	{
		FLOAT_T V0 = m_Mass * m_Vel.GetLength();
		FLOAT_T V1 = other.m_Mass * other.m_Vel.GetLength();
		char buffer[256];
		sprintf_s(buffer, "After: V0 = %.3f V1 = %.3f V0 + V1 = %.3f\n", V0, V1, V0 + V1);
		OutputDebugStringA(buffer);
	}
	#endif
}

void CBall::UpdateCollideWall(
	FLOAT_T dt, FLOAT_T maxPos, FLOAT_T ParabolaFactor,
	Vector2f &floorOffset, Vector2f &floorVel)
{
	CBallPos &posData = sm_posDataBuf[m_index];

	// 接地判定
	if (ParabolaFactor == 0)
	{
		// 地形が平面の場合
		if ((posData.m_Pos.y - posData.m_Radius) <= floorOffset.y)
		{
			// オフセットされた床に対して位置を修正
			posData.m_Pos.y = floorOffset.y + posData.m_Radius;
			// 質量無限大の床とボールの反射計算
			m_Vel.y = ((-m_Vel.y + floorVel.y) * sm_ReflectionCoef) + floorVel.y;

			// 衝突エネルギーの計算
			{
				Vector2f V = m_Vel - m_baseVel;
				sm_Ft = sm_Ft + m_Mass * V; // エネルギー量[N]を入れる
			}
		}
	}
	else
	{
		// 地形が二次式で定義される放物線の場合
		// ボールと放物線の交差判定
		// ボールに最も近い放物線(y = pf * x^2)上の座標pを求めるために、
		// (ボール中心 - p)と、pの接線の内積が0となることを表す式を作り、
		// 三次方程式を解く。虚数を持たない解が一つ得られる。
		// A*x^3+B*x+C
		// Note: 地面のオフセットに対応するため、ボール座標を放物線座標系に変換する
		Vector2f bp = posData.m_Pos - floorOffset;
		const FLOAT_T pf = ParabolaFactor;
		double c3 = -2.0f * pf * pf;
		double c2 = 0.0f;
		double c1 = (2.0f * pf * bp.y) - 1.0f;
		double c0 = bp.x;
		double xR[3], xI[3];
		cubicEquation(c3, c2, c1, c0, xR, xI);
		// ボールに最も近い放物線上の座標pを求める
		Vector2f p{ (FLOAT_T)xR[0], (FLOAT_T)(pf * xR[0] * xR[0]) };
		FLOAT_T minDist = (p - bp).GetLength();
		for (int i = 1; i < 3; i++)
		{
			if (abs(xI[i]) < 0.000001)
			{
				Vector2f pp{ (FLOAT_T)xR[i], (FLOAT_T)(pf * xR[i] * xR[i]) };
				FLOAT_T d = (pp - bp).GetLength();
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

		// ボールと、最も近い二次曲線上の点との距離が、ボールの半径より小さければ、コリジョンあり
		FLOAT_T floorDistance = bp.GetDistance(p);
		if (floorDistance <= posData.m_Radius)
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
			// Note: 地面の移動に対応するために、地面座標系での速度を考えて計算する
			Vector2f v = m_Vel - floorVel;
			m_Vel = N.reflect(v) * sm_ReflectionCoef;
			// 地面座標系での反射を、シミュレーション座標系に戻す
			m_Vel = m_Vel + floorVel;
			// ボールの位置を衝突の瞬間へ戻す
			posData.m_Pos = p + (posData.m_Radius * N);
			// ボールの座標を放物線座標系から、シミュレーション座標系へ戻す
			posData.m_Pos = posData.m_Pos + floorOffset;

			// 衝突エネルギーの計算
			{
				Vector2f V = m_Vel - m_baseVel;
				sm_Ft = sm_Ft + m_Mass * V; // エネルギー量[N]を入れる
			}
		}
	}

	// 壁との衝突判定
	if ((maxPos + floorOffset.x) <= (posData.m_Pos.x + posData.m_Radius))
	{
		m_Vel.x = ((-m_Vel.x + floorVel.x) * sm_ReflectionCoef) + floorVel.x;
		posData.m_Pos.x = (maxPos + floorOffset.x) - posData.m_Radius;
	}

	if ((posData.m_Pos.x - posData.m_Radius) <= -(maxPos - floorOffset.x))
	{
		m_Vel.x = ((-m_Vel.x + floorVel.x) * sm_ReflectionCoef) + floorVel.x;
		posData.m_Pos.x = -(maxPos - floorOffset.x) + posData.m_Radius;
	}
}

void CBall::draw(QPainter &painter)
{
	CBallPos &posData = sm_posDataBuf[m_index];

	// 単位変換
	int r = posData.m_Radius * 1000;
	Vector2 position{ (int)(posData.m_Pos.x * 1000), (int)(posData.m_Pos.y * 1000) };
	// 描画
	painter.setPen(QPen(col, 2, Qt::SolidLine, Qt::FlatCap));
	painter.drawEllipse((position.x - r), (position.y - r), r * 2, r * 2);
	//painter.drawRect(m_Rect.minX * 1000, m_Rect.minY * 1000, (m_Rect.maxX - m_Rect.minX) * 1000, (m_Rect.maxY - m_Rect.minY) * 1000);
}

void CBall::setInitialValue(Vector2f initialPos, Vector2f speed)
{
	CBallPos &posData = sm_posDataBuf[m_index];

	// 代入
	posData.m_Pos = initialPos;
	m_Vel = speed;
}

void CBall::setBall(int r, Qt::GlobalColor color, FLOAT_T mass)
{
	CBallPos &posData = sm_posDataBuf[m_index];

	posData.m_Radius = (FLOAT_T)r / 1000;
	col = color;
	m_Mass = mass;
}