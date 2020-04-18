#pragma once

#include "variable.h"

namespace Qt
{
	enum GlobalColor;
};

class QPainter;

class CBall
{
public:
	// �`��
	void draw(QPainter &painter);
	// �X�V
	void UpdateMove(float dt);
	void UpdateCollideWall(float dt, float maxPos);
	void UpdateCollideBoll(float dt, CBall &other);
	// �ŏ��̈ʒu
	void setInitialValue(Vector2f initialPos, Vector2f speed);
	// �F�Ɣ��a
	void setBall(int r, Qt::GlobalColor color, bool a);
	// �����ƈړ�
	void move(float dt);

	Vector2f m_Pos	  { 0, 0 };
	Vector2f m_Vel	  { 0, 0 };
	Vector2f m_baseVel{ 0, 0 };
	Vector2f m_basePos{ 0, 0 };
	Rect	 m_Rect	  { 0, 0 };

	float m_Mass = 1;
	float m_Radius = 20;
	Qt::GlobalColor col;

	bool A = false;
};