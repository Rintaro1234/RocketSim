#pragma once

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
	float GetDistance(Vector2f &vt);

	// ベクトルの長さを1にする
	float GetLength();

	// 長さをそろえる
	Vector2f normalize();

	// 内積
	float dot(const Vector2f &v) const
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

	Vector2f operator * (float s) const
	{
		return Vector2f{ x * s, y * s };
	}

	friend Vector2f operator * (float s, const Vector2f &v)
	{
		return Vector2f{ s * v.x, s * v.y };
	}

	Vector2f operator / (float d) const
	{
		return Vector2f{ x / d, y / d };
	}

public:
	float x = 0.0f;
	float y = 0.0f;
};

class Rect
{
public:
	// Rect を空にする
	void Empty(void);

	// Rect が空か判定する
	bool isEmpty(void);

	// Rect を球で拡張する
	void Expand(Vector2f &pos, float r);

	// Rect 同士が重なっているか判定
	bool isOverlapped(Rect &rect);

public:
	float maxX = 0.0f;
	float minX = 0.0f;
	float maxY = 0.0f;
	float minY = 0.0f;
};
