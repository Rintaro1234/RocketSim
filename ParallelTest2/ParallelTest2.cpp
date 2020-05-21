// ParallelTest2.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool findSafelyCombination(int N, int *pairLayout);
bool search(int N, int *pairLayout, int D, bool *pr);

/// <summary>
/// 自動削除する配列へのポインタ
/// </summary>
template<typename T> class autoarray
{
public:
	autoarray(T *_p) : p(_p) {}
	~autoarray() { delete[] p; }

	T &operator [] (int i) { return p[i]; }

	T *p;
};

/// <summary>
/// Nでループするインデックス
/// </summary>
inline int loopidx(int N, int i)
{
	return (i % N);
}

//
int main()
{
	// 検索
	const int N = 16;
	int pairLayout[N/2];
	for (int i = 0; i < _countof(pairLayout); i++) pairLayout[i] = -1;

	bool find = findSafelyCombination(N, pairLayout);
	
	// 検証
	if (find)
	{
		autoarray<bool> r(new bool [N]);
		memset(r.p, 0, sizeof(bool)*N);
		for (int D = 0; D < N / 2; D++)
		{
			int i = pairLayout[D];
			int ii = loopidx(N, i + (D + 1));
			r[i] = r[ii] = true;
		}
		for (int i = 0; i < N; i++)
		{
			find &= r[i];
		}
	}

	// 結果表示
	printf("\n");
	for (int i = 0; i < N / 2; i++)
	{
		printf(((i == 0) ? "%d" : ", %d"), pairLayout[i]);
	}
	printf(find? " : OK\n": " : FAIL\n");
}

/// <summary>
/// 安全に並列実行可能な組み合わせを見つける
/// </summary>
/// <param name="N">要素数</param>
/// <param name="pairLayout">組み合わせのレイアウトを返す配列</param>
/// <remarks>
/// インデックス上で距離Dの要素を組み合わせることを考える。
/// 距離は1(隣)からN/2まであり、同じ距離の組み合わせは複数回現れない。
/// pairLayout[i] は、pairLayout[i] 番目の要素と、それから距離(i+1)の要素がペアであることを示す。
/// </remarks>
bool findSafelyCombination(int N, int *pairLayout)
{
	autoarray<bool> r(new bool[N]);
	memset(r.p, 0, sizeof(bool)*N);
	return search(N, pairLayout, 1, r.p);
}

bool search(int N, int *pairLayout, int D, bool *r)
{
	// 少なくとも、この先に必要となる距離の組み合わせがあるかをチェック
	bool ok0 = true;
	for (int d = D; d <= N / 2; d++)
	{
		bool ok1 = false;
		for (int i = 0; i < N; i++)
		{
			int ii = loopidx(N, i + d);
			ok1 = (r[i] == false) && (r[ii] == false);
			if (ok1) break;
		}
		ok0 &= ok1;
		if (!ok0) break;
	}
	if (!ok0) return false;

	for (int i = 0; i < N; i++)
	{
		// 進捗表示
		if (D == 1)
		{
			printf("\r%d/%d", i, N);
		}

		// r[i]とr[i+D]がまだ未参照ならば、その組み合わせを採用。
		int ii = loopidx(N, i + D);
		if ((r[i] == false) && (r[ii] == false))
		{
			r[i] = r[ii] = true;
			pairLayout[D - 1] = i;

			if (D == (N / 2))
			{
				// もし全ての組み合わせを発見した場合には、検索処理を終了する
				return true;
			}
			else
			{
				// 次の距離の組み合わせを検索する
				bool find = search(N, pairLayout, (D + 1), r);
				if (find) return true;
			}

			// この組み合わせが駄目ならば、要素をひとつずらして試す
			r[i] = r[ii] = false;
		}
	}
	pairLayout[D - 1] = -1;

	return false;
}
