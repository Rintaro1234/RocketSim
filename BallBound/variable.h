#pragma once
#define FLOAT_T float

class Vector2
{
public:
	int x = 0;
	int y = 0;
};

class Vector2f
{
public:
	// 二点間の距離を求める
	FLOAT_T GetDistance(Vector2f &vt);

	// ベクトルの長さを1にする
	FLOAT_T GetLength();

	// 長さをそろえる
	Vector2f normalize();

	// 内積
	FLOAT_T dot(const Vector2f &v) const
	{
		return (x * v.x) + (y * v.y);
	}

	// this を法線としたベクトルの反射
	// this は長さ1.0に正規化されている前提
	Vector2f reflect(const Vector2f v) const
	{
		Vector2f r = v - 2.0f * this->dot(v) * (*this);
		return r;
	}

	Vector2f operator + (const Vector2f &v) const
	{
		return Vector2f{ x + v.x, y + v.y };
	}

	Vector2f operator - (const Vector2f &v) const
	{
		return Vector2f{ x - v.x, y - v.y };
	}

	Vector2f operator * (FLOAT_T s) const
	{
		return Vector2f{ x * s, y * s };
	}

	friend Vector2f operator * (FLOAT_T s, const Vector2f &v)
	{
		return Vector2f{ s * v.x, s * v.y };
	}

	Vector2f operator / (FLOAT_T d) const
	{
		return Vector2f{ x / d, y / d };
	}

public:
	FLOAT_T x = 0.0f;
	FLOAT_T y = 0.0f;
};

class Rect
{
public:
	// Rect を空にする
	void Empty(void);

	// Rect が空か判定する
	bool isEmpty(void);

	// Rect を球で拡張する
	void Expand(Vector2f &pos, FLOAT_T r);

	// Rect 同士が重なっているか判定
	bool isOverlapped(Rect &rect);

public:
	FLOAT_T maxX = 0.0f;
	FLOAT_T minX = 0.0f;
	FLOAT_T maxY = 0.0f;
	FLOAT_T minY = 0.0f;
};
