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
	void UpdateMove(FLOAT_T dt);
	void UpdateCollideWall(FLOAT_T dt, FLOAT_T maxPos, FLOAT_T ParabolaFactor);
	void UpdateCollideBoll(FLOAT_T dt, CBall &other);
	// �ŏ��̈ʒu
	void setInitialValue(Vector2f initialPos, Vector2f speed);
	// �F�Ɣ��a
	void setBall(int r, Qt::GlobalColor color, FLOAT_T mass);
	// �����ƈړ�
	void move(FLOAT_T dt);

	Vector2f m_Pos	  { 0, 0 };
	Vector2f m_Vel	  { 0, 0 };
	Vector2f m_baseVel{ 0, 0 };
	Vector2f m_basePos{ 0, 0 };
	Rect	 m_Rect	  { 0, 0 };

	FLOAT_T m_Mass = 10; // kg
	FLOAT_T m_Radius = 20;
	FLOAT_T m_frameCounter = 0;
	static FLOAT_T sm_ReflectionCoef;
	static Vector2f sm_Ft;
	Qt::GlobalColor col;
};