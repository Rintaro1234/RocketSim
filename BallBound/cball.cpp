#include <QPainter>
#include "cball.h"
#include "CubicEquation.h"
#include <windows.h>

float g = -9.8f;
float boxSize = 0;
float CBall::m_ReflectionCoef = 0.92f;

//-----------------------------------------------------------------------------
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

void CBall::UpdateCollideBoll(float /*dt*/, CBall &other)
{
	// �Փ˂��Ȃ���Ή������Ȃ�
	float dis = m_Pos.GetDistance(other.m_Pos);
	if ((m_Radius + other.m_Radius) < dis)
	{
		return;
	}
	
	// ������A���������Ȃ̂ɏՓ˂����肳��Ă�����A�Փ˂��Ȃ��������Ƃɂ���
	{
		Vector2f V = other.m_Vel - m_Vel;
		Vector2f D = other.m_Pos - m_Pos;
		if (0.0f <= V.dot(D)) return;
	}

	#ifdef _DEBUG
	{
		float V0 = m_Mass * m_Vel.GetLength();
		float V1 = other.m_Mass * other.m_Vel.GetLength();
		char buffer[256];
		sprintf_s(buffer, "Before: V0 = %.3f V1 = %.3f V0 + V1 = %.3f\n", V0, V1, V0 + V1);
		OutputDebugStringA(buffer);
	}
	#endif

	// �d�S�����_�Ƃ������W�n�ŁA���ˌv�Z���s���B
	// ��������ƁA���݂��̑��x�x�N�g�����A���Ό����̕��s�ɂȂ�̂ŁA�V���v���Ɍv�Z�ł���B
	// �d�S����̑��Α��x
	Vector2f Vc = ((m_Mass * m_Vel) + (other.m_Mass * other.m_Vel)) / (m_Mass + other.m_Mass);
	Vector2f Va = m_Vel - Vc;
	Vector2f Vb = other.m_Vel - Vc;

	// ���ˌv�Z
	Vector2f N = (other.m_Pos - m_Pos).normalize();
	Va = N.reflect(Va) * m_ReflectionCoef;
	Vb = N.reflect(Vb) * m_ReflectionCoef;
	// �d�S���W�n����A�ʏ�̍��W�n�ɖ߂�
	m_Vel = (Va + Vc);
	other.m_Vel = (Vb + Vc);
	// �ʒu���A�Փ˂����u�ԂɈړ�(�ߎ�)
	Vector2f ct = m_Pos + (other.m_Pos - m_Pos) * m_Radius / (m_Radius + other.m_Radius);
	m_Pos = ct - N * m_Radius;
	other.m_Pos = ct + N * other.m_Radius;

	#ifdef _DEBUG
	{
		float V0 = m_Mass * m_Vel.GetLength();
		float V1 = other.m_Mass * other.m_Vel.GetLength();
		char buffer[256];
		sprintf_s(buffer, "After: V0 = %.3f V1 = %.3f V0 + V1 = %.3f\n", V0, V1, V0 + V1);
		OutputDebugStringA(buffer);
	}
	#endif
}

void CBall::UpdateCollideWall(float /*dt*/, float maxPos, float ParabolaFactor)
{
	// �ڒn����
	/*if ((m_Pos.y - m_Radius) <= 0.0f)
	{
		m_Vel.y = -m_Vel.y * m_CoefReflection;
		m_Pos.y = m_Radius;
	}*/

	// �{�[���ƕ������̌�������
	// �{�[���ɍł��߂�������(y = pf * x^2)��̍��Wp�����߂邽�߂ɁA
	// (�{�[�����S - p)�ƁAp�̐ڐ��̓��ς�0�ƂȂ邱�Ƃ�\���������A
	// �O���������������B�����������Ȃ������������B
	// A*x^3+B*x+C
	const float pf = ParabolaFactor;
	double c3 = -2.0f * pf * pf;
	double c2 = 0.0f;
	double c1 = (2.0f * pf * m_Pos.y) - 1.0f;
	double c0 = m_Pos.x;
	double xR[3], xI[3];
	cubicEquation(c3, c2, c1, c0, xR, xI);
	// �{�[���ɍł��߂���������̍��Wp�����߂�
	Vector2f p{ (float)xR[0], (float)(pf * xR[0] * xR[0]) };
	float minDist = (p - m_Pos).GetLength();
	for (int i = 1; i < 3; i++)
	{
		if (abs(xI[i]) < 0.000001)
		{
			Vector2f pp{ (float)xR[i], (float)(pf * xR[i] * xR[i]) };
			float d = (pp - m_Pos).GetLength();
			if (d < minDist)
			{
				p = pp;
				minDist = d;
			}
		}
	}

//	char str[256];
	//sprintf(str, "x0 = %.3f + %.3fi\n", xR[0], xI[0]);
	//OutputDebugStringA(str);
	//sprintf(str, "x1 = %.3f + %.3fi\n", xR[1], xI[1]);
	//OutputDebugStringA(str);
	//sprintf(str, "x2 = %.3f + %.3fi\n", xR[2], xI[2]);
	//OutputDebugStringA(str);
	
	float floorDistance = m_Pos.GetDistance(p);
	if (floorDistance <= m_Radius)
	{
		// �ڐ�T
		// Note: �������������񎟎�(y = pf * x^2)�̔���(y = 2 * pf * x)
		Vector2f T{ 1.0f, (2.0f * pf * p.x) };

		// �@��N
		Vector2f N = Vector2f{ -T.y, T.x }.normalize();
		// �Ƃ肠�����A������̕����ɂ��낦�Ă���
		if (N.y < 0.0f) N = -1.0f * N;

		// ���x�x�N�g��(m_Vel)�𔽎˂���
		// ���ˌW����������
		m_Vel = N.reflect(m_Vel) * m_ReflectionCoef;
		// �{�[���̈ʒu���Փ˂̏u�Ԃ֖߂�
		m_Pos = p + m_Radius * N;
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

void CBall::setBall(int r, Qt::GlobalColor color, float mass)
{
	m_Radius = (float)r / 1000;
	col = color;
	m_Mass = mass;
}