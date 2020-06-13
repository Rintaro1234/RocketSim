#include "pch.h"
#include "cball.h"

using namespace std;

int compare_YandIdx(const void *a, const void *b);

//-----------------------------------------------------------------------------
// 乱数
FLOAT_T random(FLOAT_T x0, FLOAT_T x1)
{
	return x0 + (x1 - x0) / 256.0f * (FLOAT_T)(std::rand() / (RAND_MAX / 256));
}

//-----------------------------------------------------------------------------
CSimulator::CSimulator(int numBalls, FLOAT_T simSpaceW, FLOAT_T floorParabolaFactor) :
	m_numBalls(numBalls),
	m_simSpaceW(simSpaceW),
	m_floorParabolaFactor(floorParabolaFactor)
{
	// ボールのインスタンスを配列として作成
	m_ballsPos = new CBallPos[m_numBalls];
	CBall::sm_posDataBuf = m_ballsPos;
	m_balls = new CBall[m_numBalls];
	for (int i = 0; i < m_numBalls; i++)
	{
		m_balls[i].m_index = i;
	}

	// 分割した空間に何のボールが入っているか確かめるための2次配列を作成
	m_spaceGridA.m_cells = new SpaceCell[m_numCells];
	for (int i = 0; i < m_numCells; i++)
	{
		m_spaceGridA.m_cells[i].items = new CellItem[m_numBalls];
		m_spaceGridA.m_cells[i].numItems = m_numBalls;
	}
	m_spaceGridA.m_wallL = -simSpaceW / 2.0f;
	m_spaceGridA.m_wallR = simSpaceW / 2.0f;
	m_spaceGridA.m_numCells = m_numCells;

	// リセット
	m_spaceGridA.reset();
}

//-----------------------------------------------------------------------------
CSimulator::~CSimulator(void)
{
	delete[] m_balls;
	delete[] m_ballsPos;
}

//-----------------------------------------------------------------------------
// 1ステップシミュレーションをすすめる
void CSimulator::proceed(FLOAT_T dt, Vector2f &floorOffset, Vector2f &floorVel)
{
	// 地形のオフセット量を分割空間の座標に入れる
	m_spaceGridA.m_wallR = m_simSpaceW/2 + floorOffset.x;
	m_spaceGridA.m_wallL = -m_simSpaceW/2 + floorOffset.x;
	m_spaceGridA.update();

	// 空間わけのリセット
	m_spaceGridA.reset();

	// 移動計算(コリジョンは無視)
	for (int i = 0; i < m_numBalls; i++)
	{
		m_balls[i].UpdateMove(&m_spaceGridA, dt);
	}

	// 一が高い順にソートする
	#pragma omp parallel for
	for (int i = 0; i < m_numCells; i++)
	{
		SpaceCell &com = m_spaceGridA.m_cells[i];
		qsort(com.items, com.numItems, sizeof(CellItem), compare_YandIdx);
	}

	// ボール同士のコリジョン
	// Note:
	// 2つのボールが、複数の Cell にまたがっているとき、
	// 各 Cell でコリジョンを重複してコリジョンチェックと反射計算がおきる。
	// これを抑制すると、ボールが積み上がった際に、底の方のボールが片方に
	// 流れる症状が確認された。そのためこの処理は行わない。
	#pragma omp parallel for
	for (int i = 0; i < m_spaceGridA.m_numCells; i++)
	{
		const SpaceCell &com = m_spaceGridA.m_cells[i];
		for (int y = 0; y < com.numItems; y++)
		{
			const int idx0 = com.items[y].idx;
			CBallPos &ballPos0 = m_ballsPos[idx0];
			FLOAT_T buttomY = com.items[y].y - m_ballsPos[y].m_Radius * 2;
			for (int x = y + 1; x < com.numItems; x++)
			{
				if (com.items[x].y < buttomY) break;
				const int idx1 = com.items[x].idx;

				if (ballPos0.GetInterspace(m_ballsPos[idx1]) < 0.0f)
				{
					m_balls[idx0].UpdateCollideBall(dt, m_balls[idx1]);
				}
			}
		}
	}

	// ボールと床のコリジョン
	// 並行実行(マルチスレッド)
	#pragma omp parallel for
	for (int i = 0; i < m_numBalls; i++)
	{
		m_balls[i].UpdateCollideWall(dt,
			m_simSpaceW/2, m_floorParabolaFactor,
			floorOffset, floorVel);
	}
}

//-----------------------------------------------------------------------------
int compare_YandIdx(const void *a, const void *b)
{
	const CellItem &_a = *(const CellItem *)a;
	const CellItem &_b = *(const CellItem *)b;
	return (_a.y < _b.y) ? 1 : -1;
}

//-----------------------------------------------------------------------------
// シミュレーションをリセットする
void CSimulator::resetState(void)
{
	// ボールの初期位置と初速度を乱数で決める
	// (ついでに色も)
	FLOAT_T left = m_spaceGridA.m_wallL;
	FLOAT_T right = m_spaceGridA.m_wallR;
	FLOAT_T top = 0.6f;
	FLOAT_T bottom = 0.2f;
	FLOAT_T speedRange = 0.5f;
	FLOAT_T R = 0.011f;
	FLOAT_T R2 = 0.010f;
	Qt::GlobalColor ColorTable[] = { Qt::red, Qt::blue, Qt::green, Qt::cyan, Qt::yellow };
	for (int i = 0; i < m_numBalls; i++)
	{
		Qt::GlobalColor color = ColorTable[i % _countof(ColorTable)];
		m_balls[i].setBall(R, R2, color, 1);

		// すでに作成済みのボールとぶつからない位置に配置できるまで、繰り返す。
		// 100回やってダメなら諦める。
		Vector2f speed{ random(-speedRange, speedRange), 0 };
		for (int k = 0; k < 100; k++)
		{
			Vector2f Pos{ random(left + R2, right - R2), random(top, bottom) };
			m_balls[i].setInitialValue(Pos, speed);

			bool ok = true;
			for (int j = 0; j < i; j++)
			{
				FLOAT_T d = m_ballsPos[i].GetInterspace(m_ballsPos[j]);
				ok &= (0.0f <= d);
				if (!ok) break;
			}
			if (ok) break;
		}
	}
}
