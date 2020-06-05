#pragma once

#include "variable.h"

namespace Qt
{
	enum GlobalColor;
};

class QPainter;

struct YandIdx
{
	float y;
	uint32_t Idx;
};

struct lineComponent
{
	int32_t numComponent = 0;
	YandIdx * Y_Idx;
};

struct spaceGrid
{
	lineComponent * components;
	int numCells;
	float maxX;
	float minX;
	float mulIdxInvWidth;

	// 内部パラメータの更新
	void update(void)
	{
		mulIdxInvWidth = (FLOAT_T)numCells / (maxX - minX);
	}

	// リセット
	void reset(void)
	{
		for (int i = 0; i < numCells; i++)
		{
			components[i].numComponent = 0;
		}
	}
};

// CBall の位置に関するデータ
// 当たり判定を高速化するため、それに必要なデータだけを別に切り出す。
struct alignas(16) CBallPos
{
	// ボール間の隙間を求める
	float GetInterspace(const CBallPos &ball) const;

	Vector2f m_Pos{ 0, 0 };
	FLOAT_T	 m_Radius = 20;
	FLOAT_T	 m_Mass = 10; // kg
};

class CBall
{
public:
	// 描画
	void draw(QPainter &painter);
	// 更新
	void UpdateMove(spaceGrid *grid, FLOAT_T dt);
	void UpdateCollideWall(
		FLOAT_T dt, FLOAT_T maxPos, FLOAT_T ParabolaFactor,
		Vector2f &floorOffset, Vector2f &floorVel);
	void UpdateCollideBall(FLOAT_T dt, CBall &other);
	// 最初の位置
	void setInitialValue(Vector2f initialPos, Vector2f speed);
	// 色と半径
	void setBall(int r, Qt::GlobalColor color, FLOAT_T mass);
	// 落下と移動
	void move(FLOAT_T dt);

	/*
		Vector2f m_Pos	  { 0, 0 };
		FLOAT_T	 m_Radius = 20;
		FLOAT_T	 m_Mass = 10; // kg
	*/

	Vector2f m_Vel	  { 0, 0 };
	Vector2f m_baseVel{ 0, 0 };
	Vector2f m_basePos{ 0, 0 };
	int32_t	m_index = 0;
	Qt::GlobalColor col;

	static CBallPos *sm_posDataBuf;
	static FLOAT_T sm_ReflectionCoef;
	static Vector2f sm_Ft;
};