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
	void UpDate(float dt, float maxPos);
	// �ŏ��̈ʒu
	void setInitialValue(Vector2f initialPos, Vector2f speed);
	// �F�Ɣ��a
	void setBall(int r, Qt::GlobalColor color);

	Vector2f m_Pos{0, 0};
	Vector2f m_Vel{0, 0};
	float m_radius = 20;
	Qt::GlobalColor col;
};