#include <QPainter>
#include "cball.h"

float g = -9.8f;
float boxSize = 0;

void CBall::UpDate(float dt, float maxPos)
{
	boxSize = maxPos;
	Vector2f pos0 = m_Pos;
	Vector2f vel0 = m_Vel;

	// ����
	m_Vel.y = vel0.y + (g * dt);
	m_Pos.y = pos0.y + (vel0.y + m_Vel.y) * dt / 2.0f;
	m_Pos.x = pos0.x + (vel0.x + m_Vel.x) * dt / 2.0f;

	// �ڒn����
	if ((m_Pos.y - m_radius) <= 0.0f)
	{
		// �ڒn����u�Ԃ܂ł̌o�ߎ��� dt0 �����߁A�����܂ł̃V�~�����[�V�������s��
		float A = (vel0.y * vel0.y) - (2.0f * g * (pos0.y - m_radius));
		float dt0 = (-vel0.y - std::sqrt(A)) / g;
		m_Vel.y = vel0.y + (g * dt0);
//		m_Pos.y = pos0.y + (vel0.y + m_Vel.y) * dt0 / 2.0f;
		m_Pos.y = m_radius;
		m_Pos.x = pos0.x + (vel0.x + m_Vel.x) * dt0 / 2.0f;

		// ���x�x�N�g���̌����𔽓]����(�K���Ȍ�������������)
		m_Vel.y = -0.95f * m_Vel.y;
		vel0.y = m_Vel.y;

		// �c�莞�Ԃ̃V�~�����[�V�������s��
		float dt1 = dt - dt0;
		m_Vel.y = m_Vel.y + (g * dt1);
		m_Pos.y = m_Pos.y + (vel0.y + m_Vel.y) * dt1 / 2.0f;
		m_Pos.x = m_Pos.x + (vel0.x + m_Vel.x) * dt1 / 2.0f;

		// �v�Z�̌��ʁA����������艺�ɂȂ�悤�Ȃ�A�����I�ɍ��W��߂�
		// ���l�v�Z�̌덷�ɂ��A�{�[�����W�����̏�ɖ߂�Ȃ��\��������
		if ((m_Pos.y - m_radius) <= 0.0f)
		{
			m_Pos.y = m_radius;
			m_Vel.y = 0.0f;
		}
	}

	if (boxSize < (m_Pos.x + m_radius))
	{
		m_Vel.x = -m_Vel.x;
		m_Pos.x = boxSize - m_radius;
	}
	
	if ((m_Pos.x - m_radius) < -boxSize)
	{
		m_Vel.x = -m_Vel.x;
		m_Pos.x = -boxSize + m_radius;
	}
}

void CBall::draw(QPainter &painter)
{
	// �P�ʕϊ�
	int r = m_radius * 1000;
	Vector2 position{ (int)(m_Pos.x * 1000), (int)(m_Pos.y * 1000) };
	// �`��
	painter.setPen(QPen(col, 2, Qt::SolidLine, Qt::FlatCap));
	painter.drawEllipse((position.x - r), (position.y - r), r * 2, r * 2);
}

void CBall::setInitialValue(Vector2f initialPos, Vector2f speed)
{
	// ���
	m_Pos = initialPos;
	m_Vel = speed;
}

void CBall::setBall(int r, Qt::GlobalColor color)
{
	m_radius = (float)r / 1000;
	col = color;
}