﻿#pragma once

#include "variable.h"

namespace Qt
{
	enum GlobalColor;
};

class QPainter;

//-----------------------------------------------------------------------------
// 空間分割によるボール同士のコリジョン判定の高速化
class CSpaceGrid
{
public:
	// 空間の分割数
	int m_numCells;
	// 分割された空間の配列
	struct SpaceCell * m_cells;
	
	// 左右の壁のX座標
	float m_wallR;
	float m_wallL;

	// 座標から分割空間番号を算出するための係数(計算キャッシュ)
	float m_mulIdxInvWidth;

	// 内部パラメータの更新
	void update(void);

	// リセット
	void reset(void);
};

// 分割された空間の一つ
struct SpaceCell
{
	// この空間に入っているボールの数
	int32_t numItems = 0;
	// この空間に入っているボールの配列(ボールの最大数で配列を確保)
	struct CellItem * items;
};

// 空間に入っているボールを列挙するデータ構造
struct CellItem
{
	// ボールのインデックス
	uint32_t idx;
	// ボールの上端の高さ(高速化のために使用)
	float y;
};


//-----------------------------------------------------------------------------
// CBall の位置に関するデータ
// 当たり判定を高速化するため、それに必要なデータだけを別に切り出す。
class CBallPos
{
public:
	// ボール間の隙間を求める
	float GetInterspace(const CBallPos &ball) const;

	// 中心座標
	Vector2f m_Pos{ 0, 0 };
	// 半径
	FLOAT_T	 m_Radius = 20;
};

//-----------------------------------------------------------------------------
// ボール
class CBall
{
public:
	// 描画
	void draw(QPainter &painter);
	// 更新
	void UpdateMove(CSpaceGrid *grid, FLOAT_T dt);
	void UpdateCollideWall(
		FLOAT_T dt, FLOAT_T maxPos, FLOAT_T ParabolaFactor,
		Vector2f &floorOffset, Vector2f &floorVel);
	void UpdateCollideBall(FLOAT_T dt, CBall &other);
	// 最初の位置
	void setInitialValue(Vector2f initialPos, Vector2f speed);
	// ボールのパラメータを設定
	// 半径、色、質量
	void setBall(FLOAT_T r, Qt::GlobalColor color, FLOAT_T mass);
	// 落下と移動
	void move(FLOAT_T dt);

	/*
		Vector2f m_Pos	  { 0, 0 };
		FLOAT_T	 m_Radius = 20;
		FLOAT_T	 m_Mass = 10; // kg
	*/

	Vector2f m_Force  { 0, 0 };
	Vector2f m_Vel	  { 0, 0 };
	Vector2f m_baseVel{ 0, 0 };
	Vector2f m_basePos{ 0, 0 };
	FLOAT_T	 m_Mass = 10; // kg
	int32_t	m_index = 0;
	Qt::GlobalColor col;

	static CBallPos *sm_posDataBuf;
	static FLOAT_T sm_ReflectionCoef;
	static Vector2f sm_Ft;
};