#include <cmath>
#include "variable.h"
#include <xmmintrin.h>

//-----------------------------------------------------------------------------
// 二点間の距離を求める
FLOAT_T Vector2f::GetDistance(Vector2f &vt)
{
#if 1
	__m128 a, b, c, d, e, f, g;
	float dis;
	a = _mm_loadu_ps(&x);			// a = { v0.x, v0.y, -, - }
	b = _mm_loadu_ps(&vt.x);		// b = { v1.x, v1.y, -, - }
	c = _mm_sub_ps(a, b);			// c = { v0.x-v1.x, v0.y-v1.y, -, - }
	d = _mm_mul_ps(c, c);			// d = { (v0.x-v1.x)^2, (v0.y-v1.y)^2, -, - }
	e = _mm_shuffle_ps(d, d, _MM_SHUFFLE(0,1,0,1));	// d = { (v0.y-v1.y)^2, (v0.x-v1.x)^2, (v0.y-v1.y)^2, (v0.x-v1.x)^2 }
	f = _mm_add_ps(d, e);			// f = { (v0.x-v1.x)^2 + (v0.y-v1.y)^2, ... }
	g = _mm_sqrt_ss(f);				// g = { SQRT((v0.x-v1.x)^2 + (v0.y-v1.y)^2), ... }
	_mm_store_ss(&dis, g);
	return dis;
#else
	FLOAT_T dx = (vt.x - x);
	FLOAT_T dy = (vt.y - y);
	FLOAT_T dis = std::sqrt((dx * dx) + (dy * dy));
	return dis;
#endif
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