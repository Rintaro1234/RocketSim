#include <cmath>
#include "variable.h"

//-----------------------------------------------------------------------------
// 二点間の距離を求める
float Vector2f::GetDistance(Vector2f &vt)
{
	float dx = (vt.x - x);
	float dy = (vt.y - y);
	float dis = std::sqrt((dx * dx) + (dy * dy));
	return dis;
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
void Rect::Expand(Vector2f &pos, float r)
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