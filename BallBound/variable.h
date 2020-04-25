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
	// ��_�Ԃ̋��������߂�
	float GetDistance(Vector2f &vt);

	// �x�N�g���̒�����1�ɂ���
	float GetLength();

	// ���������낦��
	Vector2f normalize();

	// ����
	float dot(const Vector2f &v) const
	{
		return (x * v.x) + (y * v.y);
	}

	// this ��@���Ƃ����x�N�g���̔���
	// this �͒���1.0�ɐ��K������Ă���O��
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
	// Rect ����ɂ���
	void Empty(void);

	// Rect ���󂩔��肷��
	bool isEmpty(void);

	// Rect �����Ŋg������
	void Expand(Vector2f &pos, float r);

	// Rect ���m���d�Ȃ��Ă��邩����
	bool isOverlapped(Rect &rect);

public:
	float maxX = 0.0f;
	float minX = 0.0f;
	float maxY = 0.0f;
	float minY = 0.0f;
};
