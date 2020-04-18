#include <QPainter>
#include "cball.h"

float g = -9.8f;
float boxSize = 0;

void CBall::move(float dt)
{
	// �O��̌��ʂ��L�^
	Vector2f vel0 = m_Vel;
	// �v�Z
	m_Vel.y = vel0.y + (g * dt);
	m_Pos = m_Pos + (vel0 + m_Vel) * dt / 2.0f;
}

void CBall::UpdateMove(float dt)
{
	// �V�~�����[�V���������Z�b�g���邽�߂ɁA������Ԃ�ۑ�����
	m_baseVel = m_Vel;
	m_basePos = m_Pos;

	move(dt);

	/*// �ړ������Ƃ��͈̔͂��{�b�N�X��ɂ���
	m_Rect.Empty();
	m_Rect.Expand(m_basePos, m_radius);
	m_Rect.Expand(m_Pos, m_radius);*/
}

void CBall::UpdateCollideBoll(float dt, CBall &other)
{
	float dis = m_Pos.GetDistance(other.m_Pos);
	if (m_Radius * 2 < dis)
	{
		return;
	}

	// �d�S���x
	Vector2f Vc = ((m_Mass * m_Vel) + (other.m_Mass * other.m_Vel)) / (m_Mass + other.m_Mass);
	// �d�S����̑��Α��x
	Vector2f Va = m_Vel - Vc;
	Vector2f Vb = other.m_Vel - Vc;
	// �݂��̏d�S�����񂾃x�N�g��
	Vector2f N = m_Pos - other.m_Pos;
	Vector2f Ha = N * N.dot(Va) / N.dot(N);
	Va = Va - 2 * Ha;
	Vector2f Hb = N * N.dot(Vb) / N.dot(N);
	Vb = Vb - 2 * Hb;

	m_Vel = Va + Vc;
	other.m_Vel = Vb + Vc;

	m_Pos = m_basePos;
	other.m_Pos = other.m_basePos;
}

void CBall::UpdateCollideWall(float /*dt*/, float maxPos)
{
	// �ڒn����
	if ((m_Pos.y - m_Radius) <= 0.0f)
	{
		m_Vel.y = -m_Vel.y;
		m_Pos.y = m_Radius;
	}

	// �ǂƂ̏Փ˔���
	if (maxPos <= (m_Pos.x + m_Radius))
	{
		m_Vel.x = -m_Vel.x;
		m_Pos.x = maxPos - m_Radius;
	}

	if ((m_Pos.x - m_Radius) <= -maxPos)
	{
		m_Vel.x = -m_Vel.x;
		m_Pos.x = -maxPos + m_Radius;
	}
}

void CBall::draw(QPainter &painter)
{
	// �P�ʕϊ�
	int r = m_Radius * 1000;
	Vector2 position{ (int)(m_Pos.x * 1000), (int)(m_Pos.y * 1000) };
	// �`��
	painter.setPen(QPen(col, 2, Qt::SolidLine, Qt::FlatCap));
	painter.drawEllipse((position.x - r), (position.y - r), r * 2, r * 2);
	//painter.drawRect(m_Rect.minX * 1000, m_Rect.minY * 1000, (m_Rect.maxX - m_Rect.minX) * 1000, (m_Rect.maxY - m_Rect.minY) * 1000);
}

void CBall::setInitialValue(Vector2f initialPos, Vector2f speed)
{
	// ���
	m_Pos = initialPos;
	m_Vel = speed;
}

void CBall::setBall(int r, Qt::GlobalColor color, bool a)
{
	m_Radius = (float)r / 1000;
	col = color;
	A = a;
}