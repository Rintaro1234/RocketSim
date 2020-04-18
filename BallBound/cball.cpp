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

void CBall::Update(float dt, float maxPos, CBall other[], int arraySize, int myNumber)
{
	// �ׂ���
	int div = 16;
	//col = Qt::red;
	boxSize = maxPos;

	// �V�~�����[�V���������Z�b�g���邽�߂ɁA������Ԃ�ۑ�����
	Vector2f base_vel = m_Vel;
	Vector2f base_pos = m_Pos;

	// ����
	move(dt);

	// �ڒn����
	if ((m_Pos.y - m_radius) <= 0.0f)
	{
		// �v�Z�����Ȃ���
		m_Vel = base_vel;
		m_Pos = base_pos;

		if (A == false)
		{
			// �ڒn����܂ŗ���
			int countTime = 0;
			float div_dt = dt / (float)div;
			for (int i = 0; i < div; i++)
			{
				move(div_dt);
				if ((m_Pos.y - m_radius) <= 0.0f) break;
				countTime++;
			}
			m_Pos.y = m_radius;

			// ���x�x�N�g���̌����𔽓]����(�K���Ȍ�������������)
			m_Vel.y = -m_Vel.y * 0.92f;

			// �c�莞�Ԃ̃V�~�����[�V�������s��
			float dt1 = dt * (div - countTime) / div;
			move(dt1);
		}
		else
		{
			// �ڒn����u�Ԃ܂ł̌o�ߎ��� dt0 �����߁A�����܂ł̃V�~�����[�V�������s��
			float B = (m_Vel.y * m_Vel.y) - (2.0f * g * (m_Pos.y - m_radius));
			float dt0 = (-m_Vel.y - std::sqrt(B)) / g;
			move(dt0);

			// ���x�x�N�g���̌����𔽓]����(�K���Ȍ�������������)
			m_Vel.y = -m_Vel.y * 0.92f;

			// �c�莞�Ԃ̃V�~�����[�V�������s��
			float dt1 = dt - dt0;
			move(dt1);
		}

		// �v�Z�̌��ʁA����������艺�ɂȂ�悤�Ȃ�A�����I�ɍ��W��߂�
		// ���l�v�Z�̌덷�ɂ��A�{�[�����W�����̏�ɖ߂�Ȃ��\��������
		if ((m_Pos.y - m_radius) <= 0.0f)
		{
			m_Pos.y = m_radius;
			m_Vel.y = 0.0f;
		}
	}

	// �ړ������Ƃ��͈̔͂��{�b�N�X��ɂ���
	m_Rect.Empty();
	m_Rect.Expand(base_pos, m_radius);
	m_Rect.Expand(m_Pos, m_radius);

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
	
	// �ق��̃{�[���Ƃ������r
	for (int i = 0; i <= arraySize; i++)
	{
		// �����̂��Ƃ͌v�Z���Ȃ�
		if (i != myNumber)
		{
			bool overlapped = m_Rect.isOverlapped(other[i].m_Rect);
			if (overlapped)
			{
				m_Pos = base_pos;
				m_Vel = base_vel;
				// �����Â������ďՓ˂��邩�m�F
				int countTime = 0;
				float div_dt = dt / (float)div;
				for (int i = 0; i <= div; i++)
				{
					move(div_dt);
					float distance = m_Pos.GetDistance(other[i].m_Pos);
					countTime++;
					if (distance <= m_radius)
					{
						col = Qt::blue;
						break;
					}
				}

				// �̂�����v�Z
				move(div_dt * (div - countTime));
			}
		}
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
	painter.drawRect(m_Rect.minX * 1000, m_Rect.minY * 1000, (m_Rect.maxX - m_Rect.minX) * 1000, (m_Rect.maxY - m_Rect.minY) * 1000);
}

void CBall::setInitialValue(Vector2f initialPos, Vector2f speed)
{
	// ���
	m_Pos = initialPos;
	m_Vel = speed;
}

void CBall::setBall(int r, Qt::GlobalColor color, bool a)
{
	m_radius = (float)r / 1000;
	col = color;
	A = a;
}