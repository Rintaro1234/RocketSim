﻿#include <cmath>
#include "variable.h"

//-----------------------------------------------------------------------------
// 二点間の距離を求める
FLOAT_T Vector2f::GetDistance(Vector2f &vt)
{
	FLOAT_T dx = (vt.x - x);
	FLOAT_T dy = (vt.y - y);
	FLOAT_T dis = std::sqrt((dx * dx) + (dy * dy));
	return dis;
}

//-----------------------------------------------------------------------------
// ベクトルの長さを求める
FLOAT_T Vector2f::GetLength(void)
{
	return std::sqrt((x * x) + (y * y));
}

//-----------------------------------------------------------------------------
// ベクトルの長さを1にする
Vector2f Vector2f::normalize(void)
{
	FLOAT_T a = 1 / Vector2f{ x, y }.GetLength();
	Vector2f normalize{ (x * a), (y * a) };
	return normalize;
}

//-----------------------------------------------------------------------------
// Rect を空にする
void Rect::Empty(void)
{
	minX = maxX = minY = maxY = 0.0f;
}

//-----------------------------------------------------------------------------
// Rect が空か判定する
bool Rect::isEmpty(void)
{
	return (maxX <= minX) || (maxY <= minY);
}

//-----------------------------------------------------------------------------
// Rect を球で拡張する
void Rect::Expand(Vector2f &pos, FLOAT_T r)
{
	Vector2f sphereMin{ pos.x - r, pos.y - r };
	Vector2f sphereMax{ pos.x + r, pos.y + r };

	if (isEmpty())
	{
		// 球のバウンダリを初期矩形として代入
		minX = sphereMin.x;
		maxX = sphereMax.x;
		minY = sphereMin.y;
		maxY = sphereMax.y;
	}
	else
	{
		// 矩形を拡張する
		if (sphereMin.x < minX) minX = sphereMin.x;
		if (maxX < sphereMax.x) maxX = sphereMax.x;

		if (sphereMin.y < minY) minY = sphereMin.y;
		if (maxY < sphereMax.y) maxY = sphereMax.y;
	}
}

//-----------------------------------------------------------------------------
// Rect 同士が重なっているか判定
bool Rect::isOverlapped(Rect &rect)
{
	bool outside =
		(rect.maxX < minX) || (maxX < rect.minX) ||
		(rect.maxY < minY) || (maxY < rect.minY);
	return !outside;
}