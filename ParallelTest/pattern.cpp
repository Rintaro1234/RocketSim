#include <iostream>
#include <string>
using namespace std;

int Wave(int hz, int i);

// �[���l�ߓ񌅂�10�i���������Ԃ�
string ato2i(int x)
{
	string str;
	if (x < 10) str = "0";
	str += to_string(x);
	return str;
}

// �����̂͂ݏo���ɑ΂���ܕԂ����l�����āA�O���b�h�̃C���f�b�N�X��ϊ�����
void GetWrapIndex(int N, int x, int y, int *xx, int *yy)
{
	// �c���Ƃ��ɁA�v�f��N�����Ő܂�Ԃ�
	x = (x % N);
	y = (y % N);

	// �O���b�h�̏㔼�����w���悤�ɁA���b�v����
	if(y <= x)
	{
		*xx = x;
		*yy = y;
	}
	else
	{
		*xx = y;
		*yy = x;
	}
}

// �΂߃��C���̕��񏈗��O���[�v�����邯��
// @param N �{�[���̐�
// @param CollisionPair[][] �R���W����������s��2�̃{�[���̃y�A��\���A�񎟌��z��̃O���b�h
// @param k ���C���擪�̈ʒu(x���W)
// @param pairNo ���̃O���[�v�ԍ�
// @return ���̃O���[�v�ԍ�
int fingCollisionPairSub(int N, int **CollisionPair, int k, int pairNo);

int main(void)
{
	// �`�F�b�N�p�̕ϐ�
	const int numBalls = 8; // �����̂�

	// �R���W����������s��2�̃{�[���̃y�A���A�񎟌��z��̃O���b�h�ŕ\���B
	// �ϐ����̐����́A����ڂ̕��񏈗��O���[�v�Ŕ��肷�邩�B
	// (CollisionPair[2][3] = 5)
	// �́A2�Ԗڂ�3�Ԗڂ̃{�[�����A5��ڂ̕��񏈗��O���[�v�ŃR���W�����`�F�b�N����A�̈Ӗ��B
	// Note:
	// �O���b�h�̑Ίp�����͏d���B�������͎g��Ȃ��B
	// ��) CollisionPair[4][6] �� CollisionPair[6][4] �͓����y�A�BCollisionPair[6][4] �͎g��Ȃ��B
	int **CollisionPair = new int*[numBalls];
	for (int i = 0; i < numBalls; i++)
	{
		CollisionPair[i] = new int[numBalls];
		for (int j = 0; j < numBalls; j++) CollisionPair[i][j] = -1;
	}

	// �΂߂̃��C�������ɏ�������
	int pairNo = 0;
	for(int k = 1; k <= numBalls / 2; k++)
	{
		pairNo = fingCollisionPairSub(numBalls, CollisionPair, k, pairNo);
	}

	/*// �E�΂߉������ɐ��������Ă���
	int k = 0;
	int maxk = 0;
	// ����ڂ̂��ꂩ
	for (int i = 1; i < numBalls / 2; i++)
	{
		// x,y(x�̓{�[������2�{�܂œ���\������)
		int x = i;
		int y = 0;
		// 1���
		for (int t = 0; t < numBalls / 2; t++)
		{
			// kk�����Ԃ�k + 0�Ak + 1���J��Ԃ��Bk�����̗�̍ŏ��̒l
			int kk = k + Wave(1, t);
			// �����̂͂ݏo���ɑ΂���ܕԂ����l�����āA�O���b�h�̃C���f�b�N�X��ϊ�����
			int xx, yy;
			GetWrapIndex(numBalls, x, y, &xx, &yy);
			CollisionPair[yy][xx] = kk;
			if (maxk < kk) maxk = kk;
			if (CollisionPair[yy][xx] <= kk)
			{
				CollisionPair[yy][xx] = kk;
				if (maxk < kk) maxk = kk;
			}
			else
			{
				kk = kk + 1;
				CollisionPair[y][xx] = kk;
				if (maxk < kk) maxk = kk;
				break;
			}
			y = xx;
			x = y + i;
		}
		// �󂢂Ă���ꏊ��T��
		x = i;
		y = 0;
		for (int p = 1; CollisionPair[y][x] < 0; p++)
		{
			x = i + p;
			y = p;
		}
		for (int t = 0; t < numBalls / 2; t++)
		{
			// kk�����Ԃ�k + 0�Ak + 1���J��Ԃ��Bk�����̗�̍ŏ��̒l
			int kk = k + Wave(1, t);
			// �����̂͂ݏo���ɑ΂���ܕԂ����l�����āA�O���b�h�̃C���f�b�N�X��ϊ�����
			int xx, yy;
			GetWrapIndex(numBalls, x, y, &xx, &yy);
			CollisionPair[yy][xx] = kk;
			if (maxk < kk) maxk = kk;
			if (CollisionPair[yy][xx] <= kk)
			{
				CollisionPair[yy][xx] = kk;
				if (maxk < kk) maxk = kk;
			}
			else
			{
				kk = kk + 1;
				CollisionPair[y][xx] = kk;
				if (maxk < kk) maxk = kk;
				break;
			}
			y = xx;
			x = y + i;
		}
		k = maxk + 1;
	}*/

	// �E�΂߉������ɐ��������Ă���
	/*
	int k = 0;
	for (int i = 1; (i - 1) < numBalls / 2; i++)
	{
		//�@�����v�f�̑g�ݍ��킹�͂���Ȃ�����x������E�ɂ��炷
		int xx = i;
		int yy = 0;
		for (int j = 0; j < numBalls; j++)
		{
			// x���������炵�Ă��邽�߁A�z��ȏ�ɑ傫���Ȃ�B
			// ���̏ꍇ�Axx��yy�������Ayy��0�ɂ���B
			// ��������΁A�����̑g�ݍ��킹��������B
			if (numBalls <= xx)
			{
				if (numBalls / 2 < i + 1) break;
				xx = yy;
				yy = 0;
			}
			if ((i - 1) % 2 == 0)
			{
				// �ŏ�(y = 0)�̗v�f���ł��邾���ŏ��Ɏ��s���������ߏ������l�����s
				CollisionPair[yy][xx] = k + Wave(1, j);
			}
			else
			{
				if (i % 4 != 0)
				{
					// 2�̔{������
					CollisionPair[yy][xx] = k + Wave(2, j);
				}
				else
				{
					// 4�̔{������
					CollisionPair[yy][xx] = k + Wave(4, j);
				}
			}
			// ���̏ꏊ�ֈړ�
			xx++;
			yy++;
		}
		// 1���ꂠ����2�i�ނ��߁{2����
		k = k + 2;
	}
	*/

	// �m�F
	// 1. �S�Ẵ{�[���̑g�ݍ��킹���R���W�����`�F�b�N����Ă��邩�B
	// Note:
	// �O���b�h�̑S�ĂɁA����ڂ̕��񏈗��Ń`�F�b�N����邩�̒l�������Ă����OK�B
	// (-1 �͎��s����Ȃ����Ƃ�\���B)
	// ���łɕ��񏈗��̎��s�񐔂����ׂ�B
	bool ok = true;
	int lastParallelNo = 0;
	for(int y = 0; y < numBalls; y++)
	{
		for(int x = y + 1; x < numBalls; x++)
		{
			if(CollisionPair[y][x] == -1)
			{
				cout << "Error:���s����Ȃ��g�ݍ��킹�BBall#" << y << " - #" << x << endl;
				ok = false;
			}
			if(lastParallelNo < CollisionPair[y][x])
			{
				lastParallelNo = CollisionPair[y][x];
			}
		}
	}
	// 2. ���̕��񏈗��O���[�v�ŁA��̃{�[����2��ȏ�`�F�b�N����Ă�����G���[
	// �`�F�b�N�p�t���O�ϐ��g��
	bool *Checked = new bool [numBalls];
	for(int k = 0; k <= lastParallelNo; k++)
	{
		// �`�F�b�N�ς݃t���O�����Z�b�g
		for (int i = 0; i < numBalls; i++) Checked[i] = false;

		// ���̕��񏈗��Ń`�F�b�N�����g�ݍ��킹��񋓂��āA
		for(int y = 0; y < numBalls; y++)
		{
			for(int x = 0; x < numBalls; x++)
			{
				if(CollisionPair[y][x] == k)
				{
					// ���̕��񏈗��ł��łɃ`�F�b�N�ς݂ł���΃G���[
					if(Checked[y])
					{
						cout << "Error:" << k << "��ڂ̕��񏈗����ŕ�����Q�ƁBBall#" << y << endl;
						ok = false;
					}
					if (Checked[x])
					{
						cout << "Error:" << k << "��ڂ̕��񏈗����ŕ�����Q�ƁBBall#" << x << endl;
						ok = false;
					}
					// �`�F�b�N�ς݂ł��邱�Ƃ��}�[�N
					Checked[y] = Checked[x] = true;
				}
			}
		}
	}
	delete[] Checked;

	// �\��
	if(ok)
	{
		cout << "�����I" << endl;
	}
	// �O���b�h�\��(�c���w�b�_�t��)
	cout << "   ";
	for (int x = 0; x < numBalls; x++)
	{
		cout << ato2i(x) << ".";
	}
	cout << endl;

	for (int y = 0; y < numBalls; y++)
	{
		cout << ato2i(y) << ":";

		for (int x = 0; x < numBalls; x++)
		{
			const int a = CollisionPair[y][x];
			if(x < y)
			{
				cout << "   ";
			}
			else if(y == x)
			{
				cout << " - ";
			}
			else if (a == -1)
			{
				cout << "xx,";
			}
			else
			{
				cout << ato2i(a) << ",";
			}
		}
		cout << endl;
	}

	// �������J��
	for (int y = 0; y < numBalls; y++)
	{
		delete [] CollisionPair[y];
	}
	delete [] CollisionPair;
}

// ��������C���f�b�N�X�ɑ΂��Ď����I�ȋ�`�g(0��1)��Ԃ��ϐ�
// @param L 0��������1���A�����鐔
// @param i �C���f�b�N�X
int Wave(int L, int i)
{
	// ��`�g�̎����� 2L �Ȃ̂ŁA���͈̔͂ɐ܂�Ԃ��B
	i = i % (L * 2);
	// ��`�g�̑O��(i < L)��0�A�㔼(L <= i)��1
	return (i < L)? 0: 1;
}

// �΂߃��C���̕��񏈗��O���[�v�����邯��
// @param N �{�[���̐�
// @param CollisionPair[][] �R���W����������s��2�̃{�[���̃y�A��\���A�񎟌��z��̃O���b�h
// @param k ���C���擪�̈ʒu(x���W)
// @param pairNoBase ���̃O���[�v�ԍ�
// @return ���̃O���[�v�ԍ�
int fingCollisionPairSub(int N, int **CollisionPair, int k, int pairNoBase)
{
	// ���ꎟ����ɏ��Ԃ����Ă���\�ɑ������
	int *pair;
	pair = new int[N];
	// ��ɂ���(-1)
	for (int i = 0; i < N; i++)
	{
		pair[i] = -1;
	}

	int maxVal = k;

	// �l�����Ă���
	{
		int p = 0;
		for (bool isPairFull = false; isPairFull == false;)
		{
			int maxK = 0;
			int x = p;
			for (int i = 0; i < N / 2; i++)
			{
				// ���������悤�Ƃ���l
				int kk = Wave(1, i);
				// ���̃`�F�b�N
				int Nx;
				Nx = x + k;
				if (N <= Nx) Nx = Nx - N;

				if (pair[Nx] != kk)
				{
					pair[x] = kk;
					if (maxK < kk) maxK = kk;
				}
				else
				{
					pair[x] = maxK + 1;
					maxK = maxK + 1;
					break;
				}
				x = Nx;
			}
			// �󂫂��Ȃ����`�F�b�N
			isPairFull = true;
			for (p = 0; p < N; p++)
			{
				if (pair[p] == -1)
				{
					isPairFull = false;
					break;
				}
			}
		}
	}
	// x����y�����߁A�����Ɉꎞ�z��̏������Ă���
	int x = k - 1, y = 0;
	for (int i = 0; i < N; i++)
	{
		x = x + 1;
		y = x - k;
		// �{�[���̐����炠�ӂꂵ�Ă����ꍇ�Čv�Z
		if (N <= x)
		{
			k = N - k;
			x = y;
			y = x - k;
		}

		CollisionPair[y][x] = pair[i] + pairNoBase;
		if (maxVal < pair[i] + pairNoBase) maxVal = pair[i] + pairNoBase;
	}
	delete pair;
	return maxVal + 1;
	/*int x = k;
	int y = 0;
	int maxPairNo = pairNoBase + 1;
	for (int i = 0; i < N; i++)
	{
		// �t���b�v(�g�`)�����ϐ�
		int pairNo = pairNoBase + Wave(1, i);
		int xx, yy;
		GetWrapIndex(N, x, y, &xx, &yy);
		// ��Ɏ��̏ꏊ�������Ă���
		y = x;
		x = y + k;
		int Nx, Ny;
		GetWrapIndex(N, x, y, &Nx, &Ny);
		if ((CollisionPair[yy][xx] < 0) &&
		   ((CollisionPair[Ny][Nx] < 0) || (CollisionPair[Ny][Nx] != pairNo)))
		{
			// ���̃y�A�͖��`�F�b�N�Ȃ̂ŁA���񏈗��O���[�v�ɒǉ�����
			CollisionPair[yy][xx] = pairNo;
		}
		else
		{
			// ���łɃR���W�����`�F�b�N�ς݂̃y�A�ɖ߂����ꍇ
			CollisionPair[yy][xx] = maxPairNo + 1;
			maxPairNo = maxPairNo + 1;
			break;
		}
	}
	// ����
	// �܂��R���W�����`�F�b�N�����Ă��Ȃ��y�A��������
	for (int i = 0; i < N; i++)
	{
		y = i;
		x = i + k;
		int xx, yy;
		GetWrapIndex(N, x, y, &xx, &yy);
		if (CollisionPair[yy][xx] < 0) break;
	}
	for (int i = 0; i < N; i++)
	{
		// �t���b�v(�g�`)�����ϐ�
		int pairNo = pairNoBase + Wave(1, i);
		int xx, yy;
		GetWrapIndex(N, x, y, &xx, &yy);
		// ��Ɏ��̏ꏊ�������Ă���
		y = x;
		x = y + k;
		int Nx, Ny;
		GetWrapIndex(N, x, y, &Nx, &Ny);
		if ((CollisionPair[yy][xx] < 0) &&
		   ((CollisionPair[Ny][Nx] < 0) || (CollisionPair[Ny][Nx] != pairNo)))
		{
			// ���̃y�A�͖��`�F�b�N�Ȃ̂ŁA���񏈗��O���[�v�ɒǉ�����
			CollisionPair[yy][xx] = pairNo;
		}
		else
		{
			// ���łɃR���W�����`�F�b�N�ς݂̃y�A�ɖ߂����ꍇ
			CollisionPair[yy][xx] = maxPairNo;
			break;
		}
	}
	return maxPairNo + 1;*/
}
