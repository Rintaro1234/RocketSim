#include "variable.h"
#include "ball.h"

#include <QtWidgets>

// 1�t���[�����{�[�����ړ�����
void CBall::Update(float dt)
{
	m_DurationTime += dt;

	// ���̈ړ��ӂ̏I�_�ɓ��B�����Ƃ��A������ς���
	if (m_moveDuration <= m_DurationTime)
	{
		m_turning++;
		m_DurationTime -= m_moveDuration;

		// ������񂵂��Ƃ��̃��Z�b�g
		if (3 < m_turning) m_turning = 0;
	}
}

// �{�[����`�悷��
void CBall::Draw(QPainter &painter, int gridWidth, int gridSpan)
{
	/* �~ */

	// �ړ��o�H�̊p�̈ʒu(0��4�͓������W)
	Vector2 absCornerPos = {
		gridWidth - gridSpan / 2,
		gridWidth - gridSpan / 2
	};

	cornerPos[4] = cornerPos[0];

	// �ړ����W�̌v�Z
	Vector2 p0 = cornerPos[m_turning];
	Vector2 p1 = cornerPos[m_turning + 1];
	Vector2 dt = {
		p1.x - p0.x,
		p1.y - p0.y,
	};
	Vector2 Pos;
	Pos.x = (int)(p0.x + (dt.x * m_DurationTime) / m_moveDuration);
	Pos.y = (int)(p0.y + (dt.y * m_DurationTime) / m_moveDuration);

	// �ړ�����~�̕`��
	int Radius = gridSpan * 0.8 / 2;
	Vector2 LeftTop = {
		Pos.x - Radius,
		Pos.y - Radius,
	};
	painter.setPen(QPen(m_color, 2, Qt::SolidLine, Qt::RoundCap));
	painter.drawEllipse(LeftTop.x, LeftTop.y, 2 * Radius, 2 * Radius);
}
